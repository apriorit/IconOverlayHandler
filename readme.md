# Shell Extention Handlers for Shortcut Menu and Icon Overlay

## About 

The project is a custom shell extension including shortcut menu and icon overlay handlers.

Shell extension handlers are used to extend a set of Windows Explorer standard actions, typically to ensure correct work with special file types. With **shortcut menu handler** we add new custom items to the context menu, and with **icon handler** we change (overlay) the default icon of the file.

## Implementation

Shell extension is a COM object with its own GUID. Several standard and specific interfaces are implemented for it.

The provided solution deals with a number of typical problems:

1. Displaying custom icons when a lot of other icon overlay handlers are registered
2. Correct displaying of custom icons with the transparent background
3. Correct work of context menu handler in specific situations.

You can get more details, shell extension implementation basics, and problem situation explanations in the [related article](https://www.apriorit.com/dev-blog/357-shell-extentions-basics-samples-common-problems).

License

Licensed under the MIT license. © Apriorit.
