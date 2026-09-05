#pragma once

/* Created by Gordie Novak on 8/12/26.
 * Purpose:
 * Allows us to call the crash function on panic */
namespace pg {

    /// Throws a panic
    [[noreturn]] void panic();
}
