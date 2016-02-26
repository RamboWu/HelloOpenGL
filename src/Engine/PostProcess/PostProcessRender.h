#ifndef __POSTPROCESSRENDER_H_
#define __POSTPROCESSRENDER_H_

class PostProcessRender
{
public:
	virtual PostProcessRender* init() = 0;

	virtual void render() = 0;

	virtual void destroy() = 0;

	virtual void onChangeSize(int nWidth, int nHeight) = 0;
};

#endif