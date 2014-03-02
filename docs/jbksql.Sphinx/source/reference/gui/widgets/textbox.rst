.. include:: /helpers.rst

.. links
    none

nana::gui::textbox
==================

.. code-block:: cpp

    nana/gui/widgets/textbox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: textbox \
    : public widget_object\<widget_tag, DrawerTrigger>

    A *nana::gui::textbox* is used in a window to allow users to enter and edit
    text by typing on the keyboard.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: textbox::textbox()

    The default constructor.

.. cpp:function:: textbox::textbox(window wd, bool visible)

    Creates a textbox.

.. cpp:function:: textbox::textbox(window wd, const nana::string& title, \
    bool visible = true)

    Ditto.

.. cpp:function:: textbox::textbox(window wd, const nana::char_t* title, \
    bool visible = true)

    Ditto.

.. cpp:function:: textbox::textbox(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: textbox& textbox::append(const nana::string&, bool at_caret)

    Appends a string. If at_caret is true, the string is inserted at the
    position of caret, otherwise, it is appended at end of the textbox.

.. cpp:function:: textbox& textbox::border(bool)

    Shows/Hides the border.

.. cpp:function:: textbox& textbox::editable(bool)

    Set the textbox editable.

.. cpp:function:: bool textbox::editable() const

    Returns true if the textbox is editable.

.. cpp:function:: implement-specified-type& textbox::ext_event() const

    Retrieves the extra event object.

.. cpp:function:: bool textbox::getline(size_t n, nana::string& text) const

    Read the text in a specified line. It returns true for success.

.. cpp:function:: textbox& textbox::mask(nana::char_t c)

    Set a mask character. Text is displayed as mask character if a mask
    character is set. This is used for hiding some special text,
    such as password.

.. cpp:function:: textbox& textbox::multi_lines(bool)

    Set the textbox whether it is the mulit-line mode.

.. cpp:function:: bool textbox::multi_lines() const

    Returns true if the textbox is multi-line mode.

.. cpp:function:: bool textbox::selected() const

    Returns true if text is selected.

.. cpp:function:: void textbox::select(bool)

    Selects/Cancels all text.

.. cpp:function:: textbox& textbox::tip_string(const nana::string& str)

    Sets a tip string.

.. cpp:function:: void textbox::copy() const

    Copies the selected text into shared memory, such as clipboard under Windows.

.. cpp:function:: void textbox::paste()

    Pastes the text from shared memory.

.. cpp:function:: void textbox::del()

    Erases the selected text.

.. cpp:function:: void textbox::load(const nana::char_t* file)

    Loads a text file. Attempt to load a uncode encoding text file, be sure the
    file have a BOM header.

.. cpp:function:: void textbox::store(const nana::char_t* file) const

    Writes the text to a file with multibytes encoding.

C++03
^^^^^

.. cpp:function:: void textbox::store(const nana::char_t* file, \
    nana::unicode::t encoding) const

    Writes the text to a file with unicode encoding in little-endian.

C++11
^^^^^

.. cpp:function:: void textbox::store(const nana::char_t* file, \
    nana::unicode encoding) const

    Ditto.

Notes
-----

Note 1
^^^^^^

The return type of *ext_event()* is coming with textbox, its definition is:

.. code-block:: cpp

    struct implement-specifed
    {
        nana::fn_group<void()> first_change; //An event for the text first change after text is been opened or stored.
    };

An instance of *first_change*

Some text editor programs, such as vim, notepad++, when the text is beeing
modified, the caption of window will display a special character(such as * ~ +)
to indicate changing of text. To implement this feature, you can set a function
to the first_change for modifying the caption of your window.

Note 2
^^^^^^

The tip string is displayed when textbox is empty and it will be hidden after
textbox gets the focus.

Examples
--------

None.

See Also
--------

None.
