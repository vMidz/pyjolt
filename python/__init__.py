from .pyjolt_ext import *

from typing import TypeVar, Generic
import atexit

T = TypeVar('T')

class JoltRef(Generic[T]):
    ...

class JoltRefConst(Generic[T]):
    ...

def _cleanup_jolt_callbacks():
    """
    This function is registered to run at interpreter exit.
    It breaks the reference cycles by clearing the C++ global callbacks.
    """
    set_trace(None)
    set_assert(None)

atexit.register(_cleanup_jolt_callbacks)