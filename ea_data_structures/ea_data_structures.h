#pragma once

#if 0

BEGIN_JUCE_MODULE_DECLARATION

      ID:               ea_data_structures
      vendor:           Eyal Amir
      version:          0.0.1
      name:             Common data structures
      license:          MIT

     END_JUCE_MODULE_DECLARATION

#endif

#include "Pointers/CallbackFunc.h"

#include "Utilities/TupleUtilities.h"
#include "Utilities/StaticObjects.h"
#include "Utilities/GenericUtilities.h"

#include "Structures/FixedDynamicArray.h"

#include "Structures/OwnedVector.h"
#include "Structures/MapVector.h"
#include "Structures/SharedGUIData.h"
#include "Structures/CircularBuffer.h"
#include "Structures/BufferView.h"
#include "Structures/Filtered.h"
#include "Structures/SmallVector.h"
#include "Flags/CopyableAtomic.h"

#include "ValueWrapper/Value.h"
#include "ValueWrapper/Constructed.h"
