#pragma once
#include "NativeGameplayTags.h"

namespace Level1_Tags
{
	//~ test tags
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Test_Hello);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Test_Goodbye);

	//~ transition tags
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_Request);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_Started);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_Preloaded);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_SwapNow);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_EffectStarted);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_EffectFinished);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_Complete);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Transition_Cancelled);
} // namespace Level1_Tags
