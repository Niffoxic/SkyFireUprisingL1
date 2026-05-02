#include "Level1_Tags.h"

namespace Level1_Tags
{
	//~ test tags
    UE_DEFINE_GAMEPLAY_TAG(Event_Test_Hello,    "Event.Test.Hello");
    UE_DEFINE_GAMEPLAY_TAG(Event_Test_Goodbye,  "Event.Test.Goodbye");

#pragma region TransitionTags
	//~ transition tags
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_Request,
        "Event.Transition.Request",
        "Anyone broadcasts this to request a level transition. Payload = UDataAsset_TransitionConfig.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_Started,
        "Event.Transition.Started",
        "Director announces it accepted the request and started the transition.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_Preloaded,
        "Event.Transition.Preloaded",
        "Target sublevel finished async loading (still hidden).");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_SwapNow,
        "Event.Transition.SwapNow",
        "Internal swap signal. Can also be broadcast externally to trigger swap when SwapTrigger == OnEvent.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_EffectStarted,
        "Event.Transition.EffectStarted",
        "A presentation effect started. Useful for telemetry.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_EffectFinished,
        "Event.Transition.EffectFinished",
        "A presentation effect finished.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_Complete,
        "Event.Transition.Complete",
        "The full transition pipeline finished successfully.");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(
        Event_Transition_Cancelled,
        "Event.Transition.Cancelled",
        "The transition was cancelled (timeout, error, or explicit cancel).");
#pragma endregion
}
