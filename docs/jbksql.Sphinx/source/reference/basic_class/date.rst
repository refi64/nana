.. include:: /helpers.rst

.. links
    None

nana::date
==========

.. code-block:: cpp

    nana/datetime.hpp

.. cpp:namespace:: nana

.. cpp:class:: date

    A date operation class.

    Inherits:
        * None
    Concept:
        * None

Typedefs
--------

.. cpp:type:: date::value

    .. code-block:: cpp

        struct value
        {
            unsigned year;  //[1601 - 30827)
            unsigned month; //[1, 12]
            unsigned day;   //[1, 31]
        };

    Struct date.

Public Functions
----------------

.. cpp:function:: date::date()

    The default construction, the initialized date is today.

.. cpp:function:: date::date(int year, int month, int day)

    The initializer for a specified day.

.. cpp:function:: int date::day_of_week() const

    Return the what day.

.. cpp:function:: const value& date::read() const

    Get the current date.

.. cpp:function:: static unsinged date::year_days(unsigned year)

    Returns the number of days in the specified year.

.. cpp:function:: static unsigned date::month_days(unsigned year, \
    unsinged month)

    Returns the number of days in the specified month.

.. cpp:function:: static unsinged date::day_in_year(unsinged year, \
    unsinged monty, unsinged day)

    Returns the index of the specified day in this year, at range[1, 365] or
    [1, 366]

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :doc:`date_chooser </reference/gui/widgets/date_chooser>`
