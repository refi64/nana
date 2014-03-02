.. include:: /helpers.rst

.. links
    None

.. warning:: This page needs review.

nana::concepts::any_objective
=============================

.. code-block:: cpp

    nana/concepts.hpp

.. cpp:namespace:: nana

.. cpp:class:: any_objective

    The Any Objective is an object that may attach to some other object.

    Inherits:
        * None
    Concept:
        * None

Template Parameters
-------------------

.. code-block:: cpp

    typename IndexType

The type of index.

.. code-block:: cpp

    int Dimension

The dimension.

Typedefs
--------

.. cpp:type:: any_objective::anyobj_index_t

    The type of index. It is available if Dimension is greater than 0.

Public Functions
----------------

.. cpp:function:: void any_objective::anyobj(anyobj_index_t ..., Target&)

    **template<typename Target>**
    |br|
    Attaches to an object. The number of index is determinated by Dimension.

.. cpp:function:: Target * any_objective::anyobj(anyobj_index_t ...) const

    **template<typename Target>**
    |br|
    Retrieves the attached object. It returns a null pointer if it is empty or the specified type is not matched.

C++11
^^^^^

.. cpp:function:: void any_objective::anyobj(anyobj_index_t ..., Target &&)

    **template<typename Target>**
    |br|
    Ditto.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
