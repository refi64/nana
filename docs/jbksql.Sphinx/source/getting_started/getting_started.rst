.. todo:: Complete build informations

.. include:: /helpers.rst

.. links
.. _Nana Sourceforge website: \
    http://sourceforge.net/projects/nanapro/files/Nana/Nana%200.x/
.. _recommended version: http://sphinx-doc.org/install.html

Getting Started
===============

.. note::

    Nana is available for Windows and Linux, Mac support will follow in a future
    release.

This section will help you build Nana.

Building Nana on Windows with Visual Studio
-------------------------------------------

Nana C++11
^^^^^^^^^^

.. warning:: Nana C++11 support Microsoft Visual C++ 2012/2013

**There's two ways to get Nana:**

1. Download the latest version on `Nana Sourceforge website`_ (Recommended)
2. Clone the Nana git repository hosted on Codeplex::

    git clone https://git01.codeplex.com/nanapro

Nana C++03
^^^^^^^^^^

.. warning:: Nana C++03 support Microsoft Visual C++ 2003/2005/2008/2010

Building Nana on Windows with Mingw
-----------------------------------

Nana C++11
^^^^^^^^^^

.. warning::

    Nana C++11 support Mingw v4.7+ and depends on
    `Boost.Threads <http://www.boost.org/>`_ for threading

Nana C++03
^^^^^^^^^^

.. warning:: Nana C++03 support Mingw v3.2+

Building Nana on Linux with GCC
-------------------------------

Nana C++11
^^^^^^^^^^

.. warning:: Nana C++11 support GCC v4.7+

Nana C++03
^^^^^^^^^^

.. warning:: Nana C++03 support Mingw v3.2+ with XLib

Building the Docs
-----------------

To build the docs from sources, install the tools listed below

* Python 2.7.x (http://www.python.org/download/)
* Sphinx 1.2 or up (http://sphinx-doc.org/install.html)

*Python 3.x is also available for Sphinx however we use the*
`recommended version`_ *of python (2.7.x) in our builds.*

Clone the Nana git repository and open a console in ``/nana/docs``

Windows

.. code-block:: bash

    make.bat html

Linux

.. code-block:: bash

    make html

You will see some warnings in the output, just ignore them for now.
The resulting build is located in ``nana/docs/build``.

To do a full rebuild of the docs:

Windows

.. code-block:: bash

    make.bat clean

Linux

.. code-block:: bash

    make clean
