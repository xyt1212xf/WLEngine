#include "StreamableRenderAsset.h"

namespace ML
{
	// Intentionally empty: CStreamableRenderAsset is a pure facet (see header).
	// Kept as a TU so the class has a stable home for future streaming-state
	// members (UE: IsStreamable / streaming state), instead of growing them
	// scattered across asset types.
}
