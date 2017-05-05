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
#include <fcntl.h>

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
char		*getFileSource(int fd, t_program *p, bool loadChannels)
{
	struct stat	st;
	char		*ret;
	char		*line = (char *)(char [0xF00]){0};

	fstat(fd, &st);
	ret = (char *)mmap(NULL, st.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (ret == MAP_FAILED)
	{
		perror("mmap");
		return (NULL);
	}
	ret[st.st_size] = 0;

	if (!loadChannels)
		return ret;
	int		chan = 0;
	while (gl(line, fd))
		if (!strncmp(line, "#pragma iChannel", 16))
		{
			int		mode = 0;
			line += 18;
			strreplace(line, "//", "\0\0");
			int i = 0;
			int chan = line[-2] - '0';
			printf("chan = %i\n", chan);
			while (*line && !isspace(*line))
				p->channels[chan].file_path[i++] = *line++;
			p->channels[chan].file_path[i++] = 0;
			CHECK_ACTIVE_FLAG("linear", CHAN_LINEAR);
			CHECK_ACTIVE_FLAG("nearest", CHAN_NEAREST);
			CHECK_ACTIVE_FLAG("mipmap", CHAN_MIPMAP);
			CHECK_ACTIVE_FLAG("v-flip", CHAN_VFLIP);
			CHECK_ACTIVE_FLAG("clamp", CHAN_CLAMP);
			loadChannel(p, chan, p->channels[chan].file_path, mode);
		}
	return ret;
}

GLuint		CompileShaderFragment(t_program *p, int fd, bool fatal, bool loadChannels)
{
	GLuint		ret;
	const char	*srcs[] = {fragment_shader_text, getFileSource(fd, p, loadChannels)};

	if (srcs[1] == NULL)
	{
		if (fatal)
			exit(-1);
		else
			return (0);
	}
	ret = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ret, 2, srcs, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	return (ret);
}

GLuint		CompileShaderVertex(bool fatal)
{
	GLuint		ret;
	static bool	render_shader = false;
	const char	*vertex_main_src;

	if (render_shader)
		vertex_main_src = vertex_buffer_shader_text;
	else
		vertex_main_src = vertex_shader_text;
	ret = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ret, 1, &vertex_main_src, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	render_shader = true;
	return (ret);
}

bool		createProgram(t_program *prog, const char *file, bool fatal, bool loadChannels)
{
	int			fd = open(file, O_RDONLY);
	struct stat	st;
	GLuint		shaderVertex = CompileShaderVertex(fatal);
	GLuint		shaderFragment = CompileShaderFragment(prog, fd, fatal, loadChannels);

	fstat(fd, &st);
	prog->last_modified = st.st_mtime;
	if (shaderVertex == 0 || shaderFragment == 0)
		return (false);
	if (prog->id != 0)
		glDeleteProgram(prog->id);
	prog->id = glCreateProgram();
	printf("new program ID: %i\n", prog->id);
	glAttachShader(prog->id, shaderVertex);
	glAttachShader(prog->id, shaderFragment);
	glLinkProgram(prog->id);
	checkLink(prog->id, fatal);
	if (file != prog->program_path)
		strcpy(prog->program_path, file);
	prog->fd = fd;
	return (true);
}
