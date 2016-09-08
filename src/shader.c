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

char		*getFileSource(int fd)
{
	struct stat	st;
	fstat(fd, &st);
	char	*ret = mmap(NULL, st.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (ret == MAP_FAILED)
	{
		perror("mmap");
		return (NULL);
	}
	ret[st.st_size] = 0;
	return ret;
}

GLuint		CompileShaderFragment(int fd, bool fatal)
{
	GLuint		ret;
	const char	*srcs[] = {fragment_shader_text, getFileSource(fd)};

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
