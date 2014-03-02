.. include:: /helpers.rst

.. links
    none

nana::gui::menubar
==================

.. code-block:: cpp

    nana/gui/widgets/menubar.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: menubar \
    : public widget_object\<widget_tag, DrawerTrigger>

    A menubar widget is a toolbar at the top of window for popuping menus.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: menubar::menubar()

    The default constructor.

.. cpp:function:: menubar::menubar(window wd)

    Create a menubar at the top of specified window.

.. cpp:function:: void menubar::create(window wd)

    Create a menubar at the top of specified window.

.. cpp:function:: menu& menubar::push_back(const nana::string& text)

    Appends an item.

.. cpp:function:: menu& menubar::at(size_t index) const

    Gets the menu specified by index.

.. cpp:function:: size_t menubar::length() const

    Gets the number of menus.

Notes
-----

Note 1
^^^^^^

The widget sets the shortkey to the character following the first **&** in
text, for the item. e.g. "File(&F)" or "&File".

Examples
--------

None.

See Also
--------

* :cpp:class:`menu`
