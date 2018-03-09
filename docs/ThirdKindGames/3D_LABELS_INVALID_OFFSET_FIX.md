# 3D Labels Invalid Offset Fix

### Description
When using Renderer::DrawTextQueued to draw text labels in 3D space, the text labels will not be positioned correctly.
ProjectToScreen() returns virtual screen values in range [0-100], while the Draw2dTextWithDepth() method expects screen co-ordinates.
This change will correct sx and sy values if not in virtual screen mode (sz is depth in range [0-1], and does not need to be altered).

### Tested against
LY 1.12 StarterGame,
LY 1.12 D.R.G. Initiative.

# Test code
Call provided test code from an update function to test rendering of labels. 
Pass the entityId of an entity to track, and this will render "Label test string." with a 1 metre offset along the z axis.

```c++
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Vector3.h>
#include <MathConversion.h>
void TestLabels(AZ::EntityId entityId)
{
    static bool testDebugLabels = true;
    if (testDebugLabels && gEnv && gEnv->pRenderer)
    {
        AZ::Vector3 position;
        EBUS_EVENT_ID_RESULT(position, entityId, AZ::TransformBus, GetWorldTranslation);
        position += AZ::Vector3(0.f, 0.f, 1.f);

        SDrawTextInfo ti;
        ti.flags = eDrawText_DepthTest | eDrawText_Monospace | eDrawText_FixedSize | eDrawText_Center;
        ti.xscale = ti.yscale = 1.f;
        ti.color[0] = ti.color[1] = ti.color[2] = ti.color[3] = 1.f;
        gEnv->pRenderer->DrawTextQueued(AZVec3ToLYVec3(position), ti, "Label test string.");
    }
}
```
