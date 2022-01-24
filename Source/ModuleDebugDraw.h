#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Math/float4x4.h"
#include <WinUser.h>

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw() override;

	bool            Init();
	update_status   Update();
    update_status   PostUpdate();
	bool            CleanUp();

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);

    void DrawCuboid(vec* points, vec color);
    void DrawArrow(const vec& from, const vec& to, const vec& color, const float arrow_head_size);
private:

    static DDRenderInterfaceCoreGL* implementation;
};

#endif /* _MODULE_DEBUGDRAW_H_ */
