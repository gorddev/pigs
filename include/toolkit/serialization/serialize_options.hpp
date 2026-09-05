#pragma once
#include "toolkit/containers/SettingVar.hpp"

/* Created by Gordie Novak on 8/13/26.
 * Purpose: 
 * Allows for setting of options during serialization*/

namespace pg::ser {

    inline struct SerialWarnings {
        /// Gives a warning if there is a constant data member in an object being deserialized.
        bool const_data_member = true;
        /// Gives a warning if there is a mismatched type between the serialized data and the target object.
        bool mismatched_type = true;
        /// Gives a warning if the size of a serialized class does not match that of the target class.
        bool incompatible_size = true;
        /// Gives a warning if data cannot be copied from the serialized data to the target class because the target class is not trivially copyable.
        bool data_copy_error = true;
        /// Gives a warning if the target array is too small for the data in the serial buffer.
        bool array_too_small = true;
        /// Gives a warning if a data member in the target struct did not have corresponding serialization and thus remains uninitialized by the deserializer.
        bool missing_serial = true;
        /// Provides dev debugging information. Good to use if serializer is mysteriously failing.
        bool dev_debug = true;
        /// If true, will terminate the program if the returned Err object remains unhandled with unaddressed errors.
        bool surly = true;

    } warnings;

}
