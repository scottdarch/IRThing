
![](arcommunity.png)

# Tinker

A tiny little framework for bear-metal programming small MCUs. Tinker is written
in pure C using an objective-c-ish approach to objects. For example, all Tinker
objects are structs with initializer functions that are separate from their allocators (read more about
    this behavior in true Objective-C [in Apple's developer docs for Cocoa](https://developer.apple.com/library/ios/documentation/General/Conceptual/CocoaEncyclopedia/Initialization/Initialization.html)).

This version of Tinker does not provide any allocators.


## Components

### Button.h

Provides an event driven Button object with debouncing logic.

### RunLoop.h

An event processing primitive that superficially resembles [iOS RunLoop](https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Multithreading/RunLoopManagement/RunLoopManagement.html).

### Machine.h and State.h

A lightweight hierarchical state machine framework (Orthogonal areas, named transitions,
    and default states are not yet supported).
