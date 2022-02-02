#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Math/float4x4.h"

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw() override;

	bool            Init();
	update_status   PreUpdate();
    update_status   PostUpdate();
	bool            CleanUp();

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);

    void DrawCuboid(vec* points, vec color);
    
    // TODO: MRG.
    void DrawSphere(const vec& position, const vec& direction, const vec& color, float radius);
    void DrawCone(const vec& position, const vec& direction, float longitude, float radius, const vec& color);
    void DrawFrustum(const float4x4& matrix, vec color);
    void DrawArrow(const vec& from, const vec& to, const vec& color, const float arrow_head_size);
    void DrawLine(const vec& from, const vec& to, const vec& color);
    void DrawTriangle(const Triangle& triangle, const vec& color);
    
private:
    void MakeDrawCall();
    static DDRenderInterfaceCoreGL* implementation;
};

#endif /* _MODULE_DEBUGDRAW_H_ */
