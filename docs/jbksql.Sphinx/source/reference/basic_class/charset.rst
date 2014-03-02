.. include:: /helpers.rst

.. links
    None

nana::charset
=============

.. code-block:: cpp

    nana/charset.hpp

.. cpp:namespace:: nana

.. cpp:class:: charset

    An intelligent charset class for character code conversion.

    Inherits:
        * None
    Concept:
        * None

Public Functions
----------------

.. cpp:function:: charset::charset(const charset&)

    The copy constructor.

.. cpp:function:: charset::charset(const std::string&)

    Attempt to convert a multibytes string.

.. cpp:function:: charset::charset(const std::wstring&)

    Attempt to convert a UCS2/UCS4 string.

.. cpp:function:: charset::~charset()

    The destructor.

.. cpp:function:: charset & charset::operator=(const charset&)

    The assignment operator.

.. cpp:function:: charset::operator std::string() const

    Converts the string to multibytes string.

.. cpp:function:: charset::operator std::wstring() const

    Converts the string to UCS2/UCS4 string.


C++03
^^^^^

.. cpp:function:: charset::charset(const std::string&, nana::unicode::t)

    Attempt to convert a unicode string in byte sequence.

.. cpp:function:: std::string charset::to_bytes(nana::unicode::t)

    Converts the string to a unicode in bytes sequence with a specified unicode
    transformation format.

C++11
^^^^^

.. cpp:function:: charset::charset(charset&&)

    The move construction.

.. cpp:function:: charset & charset::operator=(const charset&&)

    The move assignment operator.

.. cpp:function:: charset::charset(std::string&&)

    Attempt to convert a multibytes string.

.. cpp:function:: charset::charset(const std::string&, nana::unicode)

    Attempt to convert a unicode string in byte sequence.

.. cpp:function:: charset::charset(std::string&&, nana::unicode)

    Ditto.

.. cpp:function:: charset::charset(const std::wstring&&)

    Attempt to convert a UCS2/UCS4 string.

.. cpp:function:: charset::operator std::string&&()

    Converts the string to multibytes string.

.. cpp:function:: charset::operator std::wstring&&()

    Converts the string to UCS2/UCS4 string.

.. cpp:function:: std::string charset::to_bytes(nana::unicode)

    Converts the string to a unicode in bytes sequenece width a specified
    unicode transformation format.

Notes
-----

None.

Examples
--------

A UTF-8 string from the socket
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    int len = ::recv(sd, buf, buflen, 0);
    textbox.caption(nana::charset(std::string(buf, len), nana::unicode::utf8));

Send the string in text to the socket as UTF-8
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    std::string utf8str = nana::charset(textbox.caption()).to_bytes(nana::unicode::utf8);
    ::send(sd, utf8str.c_str(), utf8str.size(), 0);

Convert a string to the specified multi-byte character code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    //Convert to a multibytes string through default system language.
    std::string mbstr = nana::charset(a_wstring);

    //If the default system language is English and convert
    //a Chinese unicode string to multibytes string through GB2312
    std::setlocale(LC_CTYPE, "zh_CN.GB2312"); //set::setlocale(LC_CTYPE, ".936"); call it in Windows
    std::string mbstr = nana::charset(a_wstring_with_chinese);

See Also
--------

None.
