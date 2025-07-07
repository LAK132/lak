#include "lak/opengl/shader.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

/* --- shader --- */

lak::opengl::shader::shader(shader &&other)
: _shader(lak::exchange(other._shader, 0U)), _type(other._type)
{
}

lak::opengl::shader::~shader() { clear(); }

lak::opengl::shader &lak::opengl::shader::operator=(shader &&other)
{
	lak::swap(_shader, other._shader);
	lak::swap(_type, other._type);
	other.clear();
	return *this;
}

lak::opengl::result<lak::opengl::shader> lak::opengl::shader::create(
  const lak::astring &code, GLenum shader_type)
{
	lak::opengl::get_error().discard();

	shader shdr;
	shdr._type = shader_type;
	RES_TRY_ASSIGN(shdr._shader =,
	               lak::opengl::call_checked(glCreateShader, shdr._type));

	const GLchar *cstr = code.c_str();
	RES_TRY(lak::opengl::call_checked(
	  glShaderSource, shdr._shader, 1, &cstr, nullptr));

	RES_TRY(lak::opengl::call_checked(glCompileShader, shdr._shader));

	GLint compiled;
	RES_TRY(lak::opengl::call_checked(
	  glGetShaderiv, shdr._shader, GL_COMPILE_STATUS, &compiled));

	if (!compiled)
	{
		GLint msgSize = 0;
		RES_TRY(lak::opengl::call_checked(
		  glGetShaderiv, shdr._shader, GL_INFO_LOG_LENGTH, &msgSize));

		lak::vector<char> msg;
		msg.resize(msgSize + 1U, '\0');
		RES_TRY(lak::opengl::call_checked(
		  glGetShaderInfoLog, shdr._shader, msgSize, nullptr, msg.data()));

		ERROR("Shader failed to compile");
		ERROR(msg.data());

		return lak::err_t{lak::opengl::error_code_error{GL_NO_ERROR}};
	}

	return lak::move_ok(shdr);
}

lak::opengl::shader &lak::opengl::shader::clear()
{
	if (_shader != 0)
	{
		glDeleteShader(_shader);
		if_let_err (const auto err, lak::opengl::get_error()) ERROR(err);
		_shader = 0;
	}
	return *this;
}

/* --- operator"" --- */

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_vertex_shader(const char *str, size_t)
{
	return shader::create(str, GL_VERTEX_SHADER);
}

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_fragment_shader(const char *str, size_t)
{
	return shader::create(str, GL_FRAGMENT_SHADER);
}

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_tess_control_shader(const char *str, size_t)
{
	return shader::create(str, GL_TESS_CONTROL_SHADER);
}

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_tess_eval_shader(const char *str, size_t)
{
	return shader::create(str, GL_TESS_EVALUATION_SHADER);
}

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_geometry_shader(const char *str, size_t)
{
	return shader::create(str, GL_GEOMETRY_SHADER);
}

lak::opengl::result<lak::opengl::shader>
lak::opengl::literals::operator""_compute_shader(const char *str, size_t)
{
	return shader::create(str, GL_COMPUTE_SHADER);
}

/* --- program --- */

lak::opengl::program::program(program &&other)
: _program(lak::exchange(other._program, (GLuint)0))
{
}

lak::opengl::program::~program() { clear().discard(); }

lak::opengl::program &lak::opengl::program::operator=(program &&other)
{
	lak::swap(_program, other._program);
	return *this;
}

lak::opengl::result<lak::opengl::program> lak::opengl::program::create()
{
	lak::opengl::get_error().discard();
	program prog;
	RES_TRY_ASSIGN(prog._program =, lak::opengl::call_checked(glCreateProgram));
	return lak::move_ok(prog);
}

lak::opengl::result<lak::opengl::program> lak::opengl::program::create(
  const shader &vertex, const shader &fragment)
{
	ASSERT(vertex && fragment);
	RES_TRY_ASSIGN(auto prog =, program::create());
	RES_TRY(prog.attach(vertex));
	RES_TRY(prog.attach(fragment));
	RES_TRY(prog.link());
	return lak::move_ok(prog);
}

lak::opengl::result<lak::opengl::shared_program>
lak::opengl::program::create_shared()
{
	return create().map(
	  [](lak::opengl::program &&prog)
	  { return lak::shared_ptr<program>::make(lak::move(prog)); });
}

lak::opengl::result<lak::opengl::shared_program>
lak::opengl::program::create_shared(const shader &vertex,
                                    const shader &fragment)
{
	return create(vertex, fragment)
	  .map([](lak::opengl::program &&prog)
	       { return lak::shared_ptr<program>::make(lak::move(prog)); });
}

lak::opengl::result<lak::opengl::program &> lak::opengl::program::attach(
  const shader &shader)
{
	if (shader.get() != 0)
	{
		lak::opengl::get_error().discard();
		RES_TRY(lak::opengl::call_checked(glAttachShader, _program, shader.get()));
	}
	return lak::ok_t<lak::opengl::program &>{*this};
}

lak::opengl::result<lak::opengl::program &> lak::opengl::program::link()
{
	lak::opengl::get_error().discard();
	return lak::opengl::call_checked(glLinkProgram, _program)
	  .replace<lak::opengl::program &>(*this);
}

lak::opengl::result<lak::opengl::program &> lak::opengl::program::clear()
{
	if (_program != 0)
	{
		lak::opengl::get_error().discard();
		RES_TRY(lak::opengl::call_checked(glDeleteProgram, _program));
		_program = 0;
	}
	return lak::ok_t<lak::opengl::program &>{*this};
}

lak::opengl::result<lak::opengl::program &> lak::opengl::program::use()
{
	return static_cast<const lak::opengl::program &>(*this)
	  .use()
	  .replace<lak::opengl::program &>(*this);
}

lak::opengl::result<const lak::opengl::program &> lak::opengl::program::use()
  const
{
	if (_program == 0)
		return lak::err_t{lak::opengl::error_code_error{GL_NO_ERROR}};

	lak::opengl::get_error().discard();

	return lak::opengl::call_checked(glUseProgram, _program)
	  .replace<const lak::opengl::program &>(*this);
}

lak::opengl::result<lak::optional<lak::astring>>
lak::opengl::program::link_error() const
{
	lak::opengl::get_error().discard();

	GLint linked;
	RES_TRY(lak::opengl::call_checked(
	  glGetProgramiv, _program, GL_LINK_STATUS, &linked));

	if (!linked)
	{
		GLint msgSize = 0;
		RES_TRY(lak::opengl::call_checked(
		  glGetProgramiv, _program, GL_INFO_LOG_LENGTH, &msgSize));

		lak::vector<char> msg;
		msg.resize(msgSize + 1, '\0');

		RES_TRY(lak::opengl::call_checked(
		  glGetProgramInfoLog, _program, msgSize, nullptr, msg.data()));

		return lak::ok_t<lak::optional<lak::astring>>{
		  lak::astring_view(msg.data(), msgSize).to_string()};
	}

	return lak::ok_t{lak::nullopt};
}

lak::vector<lak::opengl::shader_attribute> lak::opengl::program::attributes()
  const
{
	lak::vector<shader_attribute> attributes;

	lak::vector<GLchar> name;
	name.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTE_MAX_LENGTH>(_program)
	    .UNWRAP() +
	  1);

	attributes.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTES>(_program).UNWRAP());

	for (GLuint i = 0; i < attributes.size(); ++i)
	{
		GLsizei name_size_out = 0;
		lak::opengl::call_checked(glGetActiveAttrib,
		                          _program,
		                          i,
		                          (GLsizei)(name.size() - 1U),
		                          &name_size_out,
		                          &attributes[i].size,
		                          &attributes[i].type,
		                          name.data())
		  .UNWRAP();
		name[name_size_out + 1U] = char(0);

		attributes[i].index    = static_cast<lak::opengl::index>(i);
		attributes[i].location = static_cast<lak::opengl::location>(
		  lak::opengl::call_checked(glGetAttribLocation, _program, name.data())
		    .UNWRAP());
		attributes[i].name = name.data();
	}

	return attributes;
}

lak::vector<lak::opengl::shader_uniform> lak::opengl::program::uniforms() const
{
	lak::vector<shader_uniform> uniforms;

	lak::vector<char> name;
	name.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_UNIFORM_MAX_LENGTH>(_program)
	    .UNWRAP() +
	  1);

	uniforms.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_UNIFORMS>(_program).UNWRAP());

	for (GLuint i = 0; i < uniforms.size(); ++i)
	{
		GLsizei name_size_out = 0;
		lak::opengl::call_checked(glGetActiveUniform,
		                          _program,
		                          i,
		                          (GLsizei)(name.size() - 1U),
		                          &name_size_out,
		                          &uniforms[i].size,
		                          &uniforms[i].type,
		                          name.data())
		  .UNWRAP();
		name[name_size_out + 1U] = char(0);

		uniforms[i].index    = static_cast<lak::opengl::index>(i);
		uniforms[i].location = static_cast<lak::opengl::location>(
		  lak::opengl::call_checked(glGetUniformLocation, _program, name.data())
		    .UNWRAP());
		uniforms[i].name = name.data();
	}

	return uniforms;
}

lak::opengl::shader_attribute lak::opengl::program::attribute(
  lak::opengl::index attr_index) const
{
	shader_attribute result;

	lak::vector<char> name;
	name.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTE_MAX_LENGTH>(_program)
	    .UNWRAP() +
	  1);

	GLsizei name_size_out = 0;
	lak::opengl::call_checked(glGetActiveAttrib,
	                          _program,
	                          static_cast<GLuint>(attr_index),
	                          (GLsizei)(name.size() - 1U),
	                          &name_size_out,
	                          &result.size,
	                          &result.type,
	                          name.data())
	  .UNWRAP();
	name[name_size_out + 1U] = char(0);

	result.index    = attr_index;
	result.location = static_cast<lak::opengl::location>(
	  lak::opengl::call_checked(glGetAttribLocation, _program, name.data())
	    .UNWRAP());
	result.name = name.data();

	return result;
}

lak::opengl::shader_uniform lak::opengl::program::uniform(
  lak::opengl::index unif_index) const
{
	shader_uniform result;

	lak::vector<char> name;
	name.resize(
	  lak::opengl::get_program_value<GL_ACTIVE_UNIFORM_MAX_LENGTH>(_program)
	    .UNWRAP() +
	  1);

	GLsizei name_size_out = 0;
	lak::opengl::call_checked(glGetActiveUniform,
	                          _program,
	                          static_cast<GLuint>(unif_index),
	                          (GLsizei)(name.size() - 1U),
	                          &name_size_out,
	                          &result.size,
	                          &result.type,
	                          name.data())
	  .UNWRAP();
	name[name_size_out + 1U] = char(0);

	result.index    = unif_index;
	result.location = static_cast<lak::opengl::location>(
	  lak::opengl::call_checked(glGetUniformLocation, _program, name.data())
	    .UNWRAP());
	result.name = name.data();

	return result;
}

lak::optional<lak::opengl::index> lak::opengl::program::attrib_index(
  const GLchar *name) const
{
	auto idx = attrib_location(name);
	return idx ? lak::optional<lak::opengl::index>(
	               static_cast<lak::opengl::index>(*idx))
	           : lak::nullopt;
}

lak::optional<lak::opengl::index> lak::opengl::program::uniform_index(
  const GLchar *name) const
{
	GLuint idx;
	lak::opengl::call_checked(
	  glGetUniformIndices, _program, GLsizei(1), &name, &idx)
	  .UNWRAP();
	return idx == GL_INVALID_INDEX ? lak::nullopt
	                               : lak::optional<lak::opengl::index>{
	                                   static_cast<lak::opengl::index>(idx)};
}

lak::optional<lak::opengl::location> lak::opengl::program::attrib_location(
  const GLchar *name) const
{
	auto loc =
	  lak::opengl::call_checked(glGetAttribLocation, _program, name).UNWRAP();
	return loc == -1 ? lak::nullopt
	                 : lak::optional<lak::opengl::location>{
	                     static_cast<lak::opengl::location>(loc)};
}

lak::optional<lak::opengl::location> lak::opengl::program::uniform_location(
  const GLchar *name) const
{
	auto loc =
	  lak::opengl::call_checked(glGetUniformLocation, _program, name).UNWRAP();
	return loc == -1 ? lak::nullopt
	                 : lak::optional<lak::opengl::location>{
	                     static_cast<lak::opengl::location>(loc)};
}

lak::opengl::shader_attribute lak::opengl::program::assert_attribute(
  const GLchar *name) const
{
	return attribute(assert_attrib_index(name));
}

lak::opengl::shader_uniform lak::opengl::program::assert_uniform(
  const GLchar *name) const
{
	return uniform(assert_uniform_index(name));
}

lak::opengl::index lak::opengl::program::assert_attrib_index(
  const GLchar *name) const
{
	return static_cast<lak::opengl::index>(assert_attrib_location(name));
}

lak::opengl::index lak::opengl::program::assert_uniform_index(
  const GLchar *name) const
{
	GLuint idx;
	lak::opengl::call_checked(
	  glGetUniformIndices, _program, GLsizei(1), &name, &idx)
	  .UNWRAP();
	if (idx == GL_INVALID_INDEX) FATAL("no uniform named \"", name, "\"");
	return static_cast<lak::opengl::index>(idx);
}

lak::opengl::location lak::opengl::program::assert_attrib_location(
  const GLchar *name) const
{
	auto loc =
	  lak::opengl::call_checked(glGetAttribLocation, _program, name).UNWRAP();
	if (loc == -1) FATAL("no attribute named \"", name, "\"");
	return static_cast<lak::opengl::location>(loc);
}

lak::opengl::location lak::opengl::program::assert_uniform_location(
  const GLchar *name) const
{
	auto loc =
	  lak::opengl::call_checked(glGetUniformLocation, _program, name).UNWRAP();
	if (loc == -1) FATAL("no uniform named \"", name, "\"");
	return static_cast<lak::opengl::location>(loc);
}

const lak::opengl::program &lak::opengl::program::set_uniform(
  lak::opengl::index unif_index,
  lak::span<const void> data,
  GLsizei count,
  GLboolean transpose) const
{
	use().UNWRAP();
	ASSERT(lak::opengl::get_uint(GL_CURRENT_PROGRAM).UNWRAP() == _program);
	const auto info = uniform(unif_index);
	switch (info.type)
	{
		case GL_FLOAT:
			ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLfloat)) >= count);
			lak::opengl::call_checked(glUniform1fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_VEC2:
			ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLfloat)) >= count);
			lak::opengl::call_checked(glUniform2fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_VEC3:
			ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLfloat)) >= count);
			lak::opengl::call_checked(glUniform3fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_VEC4:
			ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLfloat)) >= count);
			lak::opengl::call_checked(glUniform4fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT2:
			ASSERT(static_cast<GLsizei>(2 * 2 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT2x3:
			ASSERT(static_cast<GLsizei>(2 * 3 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2x3fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT2x4:
			ASSERT(static_cast<GLsizei>(2 * 4 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2x4fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT3:
			ASSERT(static_cast<GLsizei>(3 * 3 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT3x2:
			ASSERT(static_cast<GLsizei>(3 * 2 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3x2fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT3x4:
			ASSERT(static_cast<GLsizei>(3 * 4 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3x4fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT4:
			ASSERT(static_cast<GLsizei>(4 * 4 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT4x2:
			ASSERT(static_cast<GLsizei>(4 * 2 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4x2fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_FLOAT_MAT4x3:
			ASSERT(static_cast<GLsizei>(4 * 3 * data.size() / sizeof(GLfloat)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4x3fv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLfloat *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE:
			ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniform1dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_VEC2:
			ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniform2dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_VEC3:
			ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniform3dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_VEC4:
			ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniform4dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT2:
			ASSERT(static_cast<GLsizei>(2 * 2 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT2x3:
			ASSERT(static_cast<GLsizei>(2 * 3 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2x3dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT2x4:
			ASSERT(static_cast<GLsizei>(2 * 4 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix2x4dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT3:
			ASSERT(static_cast<GLsizei>(3 * 3 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT3x2:
			ASSERT(static_cast<GLsizei>(3 * 2 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3x2dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT3x4:
			ASSERT(static_cast<GLsizei>(3 * 4 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix3x4dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT4:
			ASSERT(static_cast<GLsizei>(4 * 4 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT4x2:
			ASSERT(static_cast<GLsizei>(4 * 2 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4x2dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_DOUBLE_MAT4x3:
			ASSERT(static_cast<GLsizei>(4 * 3 * data.size() / sizeof(GLdouble)) >=
			       count);
			lak::opengl::call_checked(glUniformMatrix4x3dv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          transpose,
			                          (GLdouble *)data.data())
			  .UNWRAP();
			break;
		case GL_INT:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
			ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLint)) >= count);
			lak::opengl::call_checked(glUniform1iv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLint *)data.data())
			  .UNWRAP();
			break;
		case GL_INT_VEC2:
			ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLint)) >= count);
			lak::opengl::call_checked(glUniform2iv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLint *)data.data())
			  .UNWRAP();
			break;
		case GL_INT_VEC3:
			ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLint)) >= count);
			lak::opengl::call_checked(glUniform3iv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLint *)data.data())
			  .UNWRAP();
			break;
		case GL_INT_VEC4:
			ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLint)) >= count);
			lak::opengl::call_checked(glUniform4iv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLint *)data.data())
			  .UNWRAP();
			break;
		case GL_UNSIGNED_INT:
			ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLuint)) >= count);
			lak::opengl::call_checked(glUniform1uiv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLuint *)data.data())
			  .UNWRAP();
			break;
		case GL_UNSIGNED_INT_VEC2:
			ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLuint)) >= count);
			lak::opengl::call_checked(glUniform2uiv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLuint *)data.data())
			  .UNWRAP();
			break;
		case GL_UNSIGNED_INT_VEC3:
			ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLuint)) >= count);
			lak::opengl::call_checked(glUniform3uiv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLuint *)data.data())
			  .UNWRAP();
			break;
		case GL_UNSIGNED_INT_VEC4:
			ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLuint)) >= count);
			lak::opengl::call_checked(glUniform4uiv,
			                          static_cast<GLint>(info.location),
			                          count,
			                          (GLuint *)data.data())
			  .UNWRAP();
			break;
		default:
			FATAL("Unsupported type ", info.type);
	}
	return *this;
}

const lak::opengl::program &lak::opengl::program::assert_set_uniform(
  const GLchar *unif,
  lak::span<const void> data,
  GLsizei count,
  GLboolean transpose) const
{
	return set_uniform(assert_uniform_index(unif), data, count, transpose);
}
