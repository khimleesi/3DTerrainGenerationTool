#include "FrameBuffer.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
FrameBuffer::FrameBuffer()
	:	m_frameBufferObject(0)
{
	GenerateBufferObject();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
FrameBuffer::~FrameBuffer()
{
	COG_GLCALL(glDeleteFramebuffers(1, &m_frameBufferObject));

	COG_LOG("[FRAME BUFFER] Frame buffer object destroyed: ", m_frameBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Generate the buffer objects ID
*******************************************************************************************************************/
void FrameBuffer::GenerateBufferObject()
{
	COG_GLCALL(glGenFramebuffers(1, &m_frameBufferObject));

	COG_LOG("[FRAME BUFFER] Frame buffer object created: ", m_frameBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Binds the frame buffer object ID & makes it the active buffer object
*******************************************************************************************************************/
void FrameBuffer::Bind() const
{
	COG_GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject));
}


/*******************************************************************************************************************
	Unbinds the frame buffer object ID & makes it disactive
*******************************************************************************************************************/
void FrameBuffer::Unbind() const
{
	COG_GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


/*******************************************************************************************************************
	Function that sets the attachment of the FBO
*******************************************************************************************************************/
void FrameBuffer::SetAttachment(int attachment)
{
	COG_GLCALL(glDrawBuffer(attachment));
}