/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/21 00:13:40 by alelievr          #+#    #+#             */
/*   Updated  2016/07/21 00:17:52 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

void		checkCompilation(GLuint shader, bool fatal)
{
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteShader(shader);
		if (fatal)
			printf("shader compilation error, exiting\n"), exit(-1);
	}
}

void		checkLink(GLuint program, bool fatal)
{
	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteProgram(program);
		if (fatal)
			printf("link error, exiting\n"), exit(-1);
	}
}

int				gl(char *dst, const int fd)
{
	static char buff[0xF000 + 1];
	static char *ptr = buff;
	char 		*nl;
	int			r;

	r = 0;
	if ((long)ptr == 0x1l)
		ptr = buff;
	if (!(nl = strchr(ptr, '\n')))
	{
		if (ptr != buff)
		{
			strcpy(buff, ptr);
			ptr = buff + (sizeof(buff) - (ptr - buff)) - 1;
		}
		r = read(fd, ptr, (sizeof(buff)  - (ptr - buff) - 1));
		ptr[r] = '\0';
		ptr = buff;
	}
	if ((nl = strchr(ptr, '\n')))
		strlcpy(dst, ptr, nl - ptr + 1);
	ptr = nl + 1;
	return (!!nl);
}

char		*strreplace(char *str, const char *find, const char *rep)
{
	while (*str)
	{
		if (!(str = strstr(str, find)))
			break ;
		memmove(str, rep, strlen(find));
	}
	return str;
}

#define CHECK_ACTIVE_FLAG(x, y) if (strstr(line, x)) mode |= y;
char		*getFileSource(int fd, t_program *p)
{
	struct stat	st;
	char		*ret;
	char		*line = (char *)(char [0xF00]){0};

	fstat(fd, &st);
	ret = mmap(NULL, st.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (ret == MAP_FAILED)
	{
		perror("mmap");
		return (NULL);
	}
	ret[st.st_size] = 0;

	int		chan = 0;
	while (gl(line, fd))
		if (!strncmp(line, "#pragma iChannel", 16))
		{
			int		mode = 0;
			line += 18;
			strreplace(line, "//", "\0\0");
			int i = 0;
			while (*line && !isspace(*line))
				p->channels[chan].filepath[i++] = *line++;
			p->channels[chan].filepath[i++] = 0;
			CHECK_ACTIVE_FLAG("linear", CHAN_LINEAR);
			CHECK_ACTIVE_FLAG("nearest", CHAN_NEAREST);
			CHECK_ACTIVE_FLAG("mipmap", CHAN_MIPMAP);
			CHECK_ACTIVE_FLAG("linear", CHAN_VFLIP);
			CHECK_ACTIVE_FLAG("clamp", CHAN_CLAMP);
			loadChannel(p->channels + chan, p->channels[chan].filepath, mode);
			chan++;
		}
	return ret;
}

GLuint		CompileShaderFragment(int fd, bool fatal)
{
	GLuint		ret;
	t_program	p;
	const char	*srcs[] = {fragment_shader_text, getFileSource(fd, &p)};

	if (srcs[1] == NULL)
		return (0);
	ret = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ret, 2, srcs, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	return (ret);
}

GLuint		CompileShaderVertex(bool fatal)
{
	GLuint		ret;

	ret = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ret, 1, &vertex_shader_text, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	return (ret);
}

GLuint		createProgram(int fd, bool fatal)
{
	GLuint		program;

	GLuint		shaderVertex = CompileShaderVertex(fatal);
	GLuint		shaderFragment = CompileShaderFragment(fd, fatal);

	if (shaderVertex == 0 || shaderFragment == 0)
		return (0);
	program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderFragment);
	glLinkProgram(program);
	checkLink(program, fatal);

	return program;
}
