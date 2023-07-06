// framebuffer.cpp

#include "framebuffer.hpp"

#include <iostream>

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &m_id);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::assign_texture(const std::shared_ptr<Texture>& texture) const
{
	// bind
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	// pass in texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->get_id(), 0);

	// check if status is OK
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw new std::runtime_error("ERROR: Failed to complete framebuffer");
	}

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
