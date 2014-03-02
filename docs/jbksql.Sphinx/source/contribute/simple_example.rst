.. Orphan document, used as an example

:orphan:

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
