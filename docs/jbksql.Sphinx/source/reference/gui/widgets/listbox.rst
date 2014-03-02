.. include:: /helpers.rst

.. links
.. _Input Iterator: http://www.sgi.com/tech/stl/InputIterator.html
.. _See Note 1: #note-1
.. _See Note 3: #note-3

.. warning:: This page needs review.

nana::gui::listbox
==================

.. code-block:: cpp

    nana/gui/widgets/listbox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: listbox \
    : public widget_object\<widget_tag, DrawerTrigger>

    The listbox is a rectangle containing a list of strings from which the user
    can select.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
        * :cpp:class:`any_objective\<typename IndexType, int Dimension>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: listbox::resolver_interface

    .. code-block:: cpp

        template<typename T> class resolver_interface

    An interface for the item resolver. (`See Note 1`_)

.. cpp:type:: listbox::size_type

    An unsigned integer type.

.. cpp:type:: listbox::index_pair_t

    A pair type.

.. cpp:type:: listbox::cat_proxy

    A category proxy type used to access the categories.

.. cpp:type:: listbox::item_proxy

    An item proxy type used to access the items of a category.

.. cpp:type:: listbox::selection

    A container type for items.


Public Functions
----------------

.. cpp:function:: listbox::listbox()

    The default construction.

.. cpp:function:: listbox::listbox(window wd, bool visible)

    Creates a listbox.

.. cpp:function:: listbox::listbox(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: void listbox::anyobj(size_type catg, size_type index, \
    const T& t)

    **template<typename T>**
    |br|
    Sets an object for the specified item.

.. cpp:function:: T* listbox::anyobj(size_type categ, size_type index)

    **template<typename T>**
    |br|
    Returns the address of the object.

.. cpp:function:: T* listbox::anyobj(size_type categ, size_type index) const

    **template<typename T>**
    |br|
    Returns the address of the object.

.. cpp:function:: void listbox::append_header(const nana::string& text, \
    unsigned width)

    Appends an item into header column with the text and the specified width.

.. cpp:function:: cat_proxy listbox::append(const nana::string&)

    Appends a new category at the end.

.. cpp:function:: cat_proxy listbox::at(std::size_t pos) const

    Returns a proxy which refers to the pos'th category.

.. cpp:function:: item_proxy listbox::at(std::size_t pos, \
    std::size_t index) const

    Returns a proxy which refers to the index'th item in the pos'th category.

.. cpp:function:: void listbox::auto_draw(bool)

    Set the listbox auto-draw state. If it is true, the listbox will redraw
    automatically after an operation.

.. cpp:function:: void listbox::checkable(bool)

    Enables or disables the checkable state of the listbox.

.. cpp:function:: selection listbox::checked() const

    Returns the items which are checked.

.. cpp:function:: void listbox::clear(size_t cat)

    Removes the items which are in a specified category.

.. cpp:function:: void listbox::clear()

    Removes the items of all categories.

.. cpp:function:: item_proxy listbox::erase(item_proxy)

    Erases an item.

.. cpp:function:: void listbox::erase(size_type cat)

    Erases a category.

.. cpp:function:: void listbox::erase()

    Erases all categories.

.. cpp:function:: void listbox::icon(size_type cat, size_type pos, \
    const nana::paint::image&)

    Sets an icon for a specified item.

.. cpp:function:: nana::paint::image listbox::icon(size_type cat, \
    size_type pos) const

    Gets an icon of a specified item.

.. cpp:function:: void listbox::insert(size_t cat, size_t pos, \
    const nana::string&)

    Insert a new item.

.. cpp:function:: void listbox::move_select(bool upwards)

    Selects an item besides the current selected item.

.. cpp:function:: void listbox::resolver(const Resolver&)

    **template<typename Resolver>**
    |br|
    Sets an item resolver.

.. cpp:function:: selection listbox::selected() const

    Get the indexs of all the selected items.

.. cpp:function:: void listbox::show_header(bool)

    Set the visible of header column.

.. cpp:function:: size_type listbox::size_categ() const

    Get the number of category item.

.. cpp:function:: size_type listbox::size_item() const

    Returns the number of default category item.

.. cpp:function:: size_type listbox::size_item(size_type categ) const

    Returns the number of items which are the specified cateory.

.. cpp:function:: bool listbox::visible_header() const

    Retrieves the visible of header column.

C++03
-----

.. warning:: The following declaration is not parsed by Sphinx.

.. cpp:function:: void listbox::set_sort_compare(size_type subindex, \
    nana::functor<bool(const nana::string&, nana::any*, const nana::string&, \
    nana::any*, bool reverse)>& strick_ordering)

    Sets a strick weak ordering comparer for a column. (`See Note 3`_)

C++11
-----

.. warning:: The following declaration is not parsed by Sphinx.

.. cpp:function:: void listbox::set_sort_compare(size_type subindex, \
    std::function<bool(const nana::string&, nana::any*, const nana::string&, \
    nana::any*, bool reverse)> strick_ordering)

    Sets a strick weak ordering comparer for a column. (`See Note 3`_)


Category Proxy (on concept of `Input Iterator`_)
------------------------------------------------

.. cpp:function:: item_proxy listbox::cat_proxy::append(const T& t)

    **template<typename T>**
    |br|
    Appends a new item, and the object 't' will be resolved by current resolver.

.. cpp:function:: std::size_t listbox::cat_proxy::columns() const

    Return the number of columns.

.. cpp:function:: void listbox::cat_proxy::push_back(const nana::string&)

    Inserts a new item at the end.

.. cpp:function:: void listbox::cat_proxy::push_back(nana::string&&)

    since C++11

.. cpp:function:: item_proxy listbox::cat_proxy::begin() const

    Returns an item_proxy referring to the beginning of the category.

.. cpp:function:: item_proxy listbox::cat_proxy::end() const

    Returns an item_proxy referring to the end of the category.

.. cpp:function:: item_proxy listbox::cat_proxy::cbegin() const

    Returns an item_proxy referring to the beginning of the category.

.. cpp:function:: item_proxy listbox::cat_proxy::cend() const

    Returns an item_proxy referring to the end of the category.

.. cpp:function:: item_proxy listbox::cat_proxy::at(std::size_t pos) const

    Returns an item_proxy referring to the pos'th item.

.. cpp:function:: std::size_t listbox::cat_proxy::size() const

    Returns the number of items.

Item Proxy (on concept of `Input Iterator`_)
--------------------------------------------

.. cpp:function:: item_proxy & listbox::item_proxy::bgcolor(nana::color_t)

    Set the background color for the item.

.. cpp:function:: nana::color_t listbox::item_proxy::bgcolor() const

    Get the background color of the item.

.. cpp:function:: item_proxy& listbox::item_proxy::fgcolor(nana::color_t)

    Set the foreground color of the item.

.. cpp:function:: nana::color_t listbox::item_proxy::fgcolor() const

    Get the foreground color of the item.

.. cpp:function:: item_proxy& listbox::item_proxy::check(bool)

    Check/Uncheck the item.

.. cpp:function:: bool listbox::item_proxy::checked() const

    Determinte whether the item is checked.

.. cpp:function:: std::size_t listbox::item_proxy::columns() const

    Returns the number of columns.

.. cpp:function:: std::pair<std::size_t, \
    std::size_t> listbox::item_proxy::pos() const

    Returns the index pair of the item.

.. cpp:function:: item_proxy & listbox::item_proxy::resolve(const T& t)

    **template<typename T>**
    |br|
    Set the item with the specified object t, and the t will be
    resolved by current resolver.

.. cpp:function:: T listbox::item_proxy::resolver() const

    **template<typename T>**
    |br|
    Get a T object which is resolved by the item.

.. cpp:function:: item_proxy & listbox::item_proxy::select(bool)

    Select the item.

.. cpp:function:: bool listbox::item_proxy::selected() const

    Determines whether the item is selected.

.. cpp:function:: item_proxy & listbox::item_proxy::text(std::size_t pos, \
    const nana::string&)

    Set the text of a specified column.

.. cpp:function:: item_proxy & listbox::item_proxy::text(std::size_t pos, \
    nana::string&&)

    since C++11
    |br|
    Set the text of a specified column.

.. cpp:function:: nana::string listbox::item_proxy::text(std::size_t pos) const

    Get the text of a specified column.

.. cpp:function:: T* listbox::item_proxy::value_ptr() const

    **template<typename T>**
    |br|
    Returns an address of the user object. If the user object is empty
    or the specified type is mismatched, it returns a nullptr.

.. cpp:function:: T& listbox::item_proxy::value() const

    **template<typename T>**
    |br|
    Returns a reference of the user object. If the user object is not
    existing or the specified type is mismatched, it throws an exception.

.. cpp:function:: item_proxy & listbox::item_proxy::value(const T&)

    **template<typename T>**
    |br|
    Set a user object.

.. cpp:function:: bool listbox::item_proxy::operator==(\
    const nana::string& s) const

    Returns true when the text of the first column is equal to the s.

.. cpp:function:: bool listbox::item_proxy::operator==(const char* s) const

    Ditto.

.. cpp:function:: bool listbox::item_proxy::operator==(const wchar_t* s) const

    Ditto.

Notes
-----

Note 1
^^^^^^

The resolver is used to resolute an object of the specified type for a listbox
item.

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/listbox.hpp>

    struct person
    {
        nana::string name;
        bool gender;
        unsigned age;
    };

    class resolver
        : public nana::gui::listbox::resolver_interface<person>
    {
        nana::string decode(std::size_t i, const person& pr) const
        {
            std::stringstream ss;
            switch(i)
            {
            case 0:
                return pr.name;
            case 1:
                return (pr.gender ? STR("Male") : STR("Female"));
            case 2:
                ss<<pr.age;
                return nana::charset(ss.str());
            }
            return nana::string();
        }

        void encode(person& pr, std::size_t i, const nana::string& s)
        {
            std::stringstream ss;
            switch(i)
            {
            case 0:
                pr.name = s;
                break;
            case 1:
                pr.gender = (s == STR("Male"));
                break;
            case 2:
                ss<<static_cast<std::string>(nana::charset(s));
                ss>>pr.age;
                break;
            }
        }
    }

    int main()
    {
        using namespace nana::gui;

        form fm;
        listbox  lb(fm, nana::rectangle(10, 10, 280, 120));
        lb.append_header(STR("Name"), 100);
        lb.append_header(STR("Gender"), 60);
        lb.append_header(STR("Age"), 60);

        lb.resolver(resolver()); //Pass the user defined resolver to the listbox

        person pr;
        pr.name = STR("Peter");
        pr.gender = true;
        pr.age = 10;

        lb.at(0).append(pr);  //person to item. resolver::decode() is required

        lb.at(0, 0).resolve_to(pr); //item to person. resolver::encode() is required

        fm.show();
        exec();
    }

Note 2
^^^^^^

The user object is an object which needs to be associated with an item.

.. code-block:: cpp

    int main()
    {
        using namespace nana::gui;
        form fm;
        listbox lb(fm, nana::rectangle(10, 10, 280, 120));
        lb.append_header(STR("Header"), 200);
        lb.append_item(STR("int"));
        lb.append_item(STR("double"));

        lb.at(0, 0).value(10);
        lb.at(0, 1).value(0.1);

        int i = lb.at(0, 0).value<int>();          //it throws an exception if there is not an int object to be bound.
        int * p = lb.at(0, 0).value_ptr<double>(); //it returns a nullptr if there is not an double object to be bound.

        fm.show();
        exec();
    }

Note 3
^^^^^^

A sort compare is used for sorting the items. It is a strick weak ordering
comparer that must meet the requirement:

* Irreflexivity(comp(x, x) returns false) and
* antisymmetry(comp(a, b) != comp(b, a) returns true)

A simple example.

.. code-block:: cpp

    bool sort_compare(const nana::string& s1, nana::any*, const nana::string& s2, nana::any*, bool reverse)
    {
        return (reverse ? s1 > s2 : s1 < s2);
    }

    listbox.set_sort_compare(0, sort_compare);

The listbox supports attaching a customer's object for each item, therefore the
items can be sorted by comparing these customer's object.

.. code-block:: cpp

    bool sort_compare(const nana::string&, nana::any* o1, const nana::string&, nana::any* o2, bool reverse)
    {
        if(o1 && o2) //some items may not attach a customer object.
        {
            int * i1 = o1->get<int>();
            int * i2 = o2->get<int>();
            return (i1 && i2 && (reverse ? *i1 > *i2 : *i1 < *i2)); //some types may not be int.
        }
        return false;
    }

    listbox.at(0, 0).value(10); //the type of customer's object is int.
    listbox.at(0, 0).value(20);

Examples
--------

None.

See Also
--------

None.
