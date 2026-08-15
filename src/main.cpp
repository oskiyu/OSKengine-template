#include <OSKengine/Platforms.h>

#include "GameMin.h"

#ifndef OSK_ANDROID
#include <OSKengine/GameEntry.h>
int main() {
	OSK::GameEntry<GameMin>(); // MODIFICAR
	return 0;
}
#else
#include <OSKengine/GameEntryAndroid.h>
OSK::UniquePtr<OSK::IGame> game = OSK::MakeUnique<GameMin>(); // ¿extern?
#define OSK_ANDROID_MAIN
#include <OSKengine/GameEntry.h>
#endif
