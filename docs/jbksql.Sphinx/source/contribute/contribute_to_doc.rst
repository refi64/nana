.. include:: /helpers.rst

.. links
.. _Sphinx: http://sphinx-doc.org/
.. _Sphinx Website (TOC tree): http://sphinx-doc.org/markup/toctree.html
.. _Building the docs: ../getting_started/getting_started.html#building-the-docs
.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Todolist page: todos.html
.. _Domains: http://sphinx-doc.org/domains.html
.. _C++ Domain: http://sphinx-doc.org/domains.html#id2
.. _Nana C++ Library: https://nanapro.codeplex.com/

Contribute to Nana Documentations
=================================

Nana uses `Sphinx`_ documentation system to build the documentations.
Sphinx have a basic C++ support (class, functions, types, members) and
the whole docs takes advantage of this feature.

Sphinx does not go thru the source code to search for doc comments, this
feature is only available for the python language (autodoc).

What you need ?
---------------

Refer to `Building the docs`_

Writing documentations
----------------------

Sphinx uses `reStructuredText`_ as its markup language, take a look at the
basics on the sphinx website: http://sphinx-doc.org/rest.html#rst-primer

The best way to get started is to watch the source code of the existing doc and
use it as your base.

**If you don't know where to start you can visit the** `Todolist page`_

Guidelines
^^^^^^^^^^

#. Indent using **4 spaces**.
#. Try to stay as much as possible on a **80 chars per line** scheme (except for
   source code).
#. Always include the ``.. include:: /helpers.rst`` file at begin.
#. Put hyperlinks below the include directive.
#. Use ``:doc:`link name </path/to/rst_file>``` (without file suffixe) for
   internal links.
   If you refer to an existing class use ``:cpp:class:`classname``` instead.
#. Use **underscore** in *file/folder/image* names.
#. Use **.rst** for the file extension.
#. Put images in the folder **/_static/images**
#. Use absolute path for images ``.. image:: /_static/images/your_image.png``

Simple Example
""""""""""""""

.. code-block:: rest

    .. include:: /helpers.rst

    .. links
    .. _A Link: http://example.org

    Page main title
    ===============

    This is `A Link`_ to an external website.

    Sub title
    ---------

    This is :doc:`A link </reference/gui/widgets/button>` to an internal
    doc page.

    Sub sub title
    ^^^^^^^^^^^^^

    This is :cpp:class:`A link <button>` to the button class with a custom link
    name.

    Paragraph title
    """""""""""""""

    Inline ``code``

    C++ code block sample:

    .. code-block:: cpp

        #include <iostream>

        int main()
        {
            std::cout << "Hello world!" << std::endl;
            return 0;
        }

    C++ code block with line numbers and line 1,5 highlighted:

    .. code-block:: cpp
        :linenos:
        :emphasize-lines: 1,5

        #include <iostream>

        int main()
        {
            std::cout << "Hello world!" << std::endl;
            return 0;
        }

    Centered image:

    .. image:: /_static/images/appearance_01.png
        :align: center

Result :doc:`Simple Example </contribute/simple_example>`

**Don't get confused with Spinx comment style and directives**

.. code-block:: rest

    .. This is a comment

    ..
        This is a
        multi lines comment

Directives always ends with ``::``

.. code-block:: rest

    .. image::

    .. include::

    .. cpp:function::

Documenting Classes
-------------------

In this section i will give you a general overview on how Nana classes are
documented, you should read this if you intent to document
classes/functions/types.

Sphinx have different `Domains`_ for each languages, the `C++ Domain`_
provide:

.. rst:directive:: .. cpp:class:: signatures
                   .. cpp:function:: signatures
                   .. cpp:member:: signatures
                   .. cpp:type:: signatures

Classes
^^^^^^^

.. code-block:: rest

    .. cpp:class:: ClassName : public ParentClass

        Class description

You can use namespaces too:

.. code-block:: rest

    .. cpp:class:: foo::bar::ClassName : public foo::bar::ParentClass

        Class description

Template classes:

.. code-block:: rest

    .. cpp:class:: foo::bar::ClassName\<param>

        Opening angle bracket needs to be escaped with an antislash

Antislash can also be used for multi lines declarations:

.. code-block:: rest

    .. cpp:class:: foo::bar::VeryVeryLooooooongClassName \
        : public foo::bar::ParentClass

        Class description

To reference any declared classes, functions, types, you need to use the
referencing syntaxe:

.. code-block:: rest

    .. Simple call, use classname for hyperlink name

    :cpp:class:`ClassName`

    .. Custom call, use custom name for hyperlink name

    :cpp:class:`My Custom Name <ClassName>`

    .. Namespaced

    :cpp:class:`foo::bar::ClassName`
    :cpp:class:`ClassName <foo::bar::ClassName>`

    ..
        Class with template parameters, remember to escape the opening angle
        bracket if you use the custom name version.

    :cpp:class:`CustClass <class\<param1, param2>>`

Functions
^^^^^^^^^

.. code-block:: rest

    .. cpp:function:: button()

        Constructor.

    .. cpp:function:: button(nana::gui::window wd, bool visible = true)

        Desc.

        nana::gui::window wd
            param1 desc.

        bool visible
            param2 desc.

    .. cpp:function:: bool namespace::func_example() const

        ...

    .. Referencing functions

    The function :cpp:func:`namespace::func_example()` text...

Types
^^^^^

.. code-block:: rest

    .. cpp:type:: Event

        Desc.

    .. cpp:type:: namespace::Event

        Desc.

    .. Referencing types

    The type :cpp:type:`namespace::Event` text...

Members
^^^^^^^

.. code-block:: rest

    .. cpp:member:: nana::gui::button class::btn

        Desc.

    .. Referencing members

    The member :cpp:member:`class::btn` text...

Choices made
------------

There's 2 ways to build an index with declared classes, funcs, types, in Sphinx.
First example:

.. code-block:: rest

    .. cpp:class:: MyClass

        Class desc.

        .. cpp:function:: MyClass()

            Constructor.

        .. cpp:member:: std::string myString

            Desc.

In the above example the constructor and the ``myString`` member will be indexed
like this:

* MyClass (C++ class)
* MyClass::MyClass (C++ function)
* MyClass::myString (C++ member)

Notice the *auto added* ``MyClass::`` for the constructor and the string member.
By indenting functions, members inside the class, Sphinx knows they are part
of it. This is nice and it's exactly what we are looking for.

However this first way of indexing is not flexible.
Let's say i want a nice title before functions declarations and members
declarations... **i can't!**

Now take a look at the second way (the way used in Nana docs):

.. code-block:: rest

    .. include:: /helpers.rst

    .. links
        none

    nana::gui::button
    =================

    .. code-block:: cpp

        nana/gui/widgets/button.hpp

    .. cpp:class:: button \
        : public widget_object\<widget_tag, DrawerTrigger>

        A Button widget.

    Typedefs
    --------

    C++03
    ^^^^^

    .. cpp:type:: button::state::t

        .. code-block:: cpp

            struct state
            {
                enum t { normal, focused, highlight, pressed, disabled };
            };

        Defines the state of button actions.

    C++11
    ^^^^^

    .. cpp:type:: button::state

        .. code-block:: cpp

            enum class state
            {
                normal, focused, highlight, pressed, disabled
            };

        Defines the state of button actions.

    Public Functions
    ----------------

    .. cpp:function:: button::button()

        Constructor.

    .. cpp:function:: void button::icon(const nana::paint::image&)

        Sets an icon image.

        const nana::paint::image&
            See :cpp:class:`image`

This an exagerated example i agree, but it gives you a good idea on how
classes are documented in Nana.

**The downside of doing this way is adding** ``ClassName::`` **on every funcs,
types and members. If you forgot they will appears in index like globals.**

Also remember if you reference a func with ``:cpp:func:`` (or any other types)
you have to give the full namespaced name for that func
``:cpp:func:`class::funcname```.

Submit your changes
-------------------

Do a fork of `Nana C++ Library`_ on codeplex, commit your changes in the fork
and do a Pull Request in the main repo (make sure your fork is up to date with
the main repo before doing a Pull Request).

Start your commit message with **Docs: Your commit message**.

If you make a new documentation page that does not fit in any existing
categories/folders, juste make a new folder with an explicit name.

.. todo::

    A toctree explanation to include documents in main page.
    For now you can learn more about it on `Sphinx Website (TOC tree)`_
