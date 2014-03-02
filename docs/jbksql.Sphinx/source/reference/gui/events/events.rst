.. include:: /helpers.rst

.. links
    None

nana::gui::events
=================

.. code-block:: cpp

    nana/gui/detail/eventinfo.hpp

.. cpp:namespace:: nana::gui

Nana.GUI provides some raw events, such as click, mouse_move and so on. The most
of these events can work with all widgets implemented by Nana.GUI, but some of
them are individual, like unload for root widget, elapse for timer.


Every widget has an interface for registering an event, it's named *make_event*.

.. code-block:: cpp

    template<typename Event, typename Function>
    event_handle make_event(Function) const;

    template<typename Event, typename Class, typename Concept>
    event_handle make_event(Class& object, void (Concept::*mf)(const nana::gui::eventinfo&));

    template<typename Event, typename Class, typename Concept>
    event_handle make_event(Class& object, void(Concept::*mf)());

These template function calls with explicitly specified the first template
parameter, the Event is a type defined by Nana.GUI in *nana::gui::events*.

The events are:

.. cpp:type:: events::click

    A mouse click event.

.. cpp:type:: events::dbl_click

    A mouse double click event.

.. cpp:type:: events::mouse_enter

    A mouse enters a widget.

.. cpp:type:: events::mouse_move

    A mouse moves over a widget.

.. cpp:type:: events::mouse_leave

    A mouse leaves a widget.

.. cpp:type:: events::mouse_down

    A mouse button is pressed on a widget.

.. cpp:type:: events::mouse_up

    A mouse button is released on a widget.

.. cpp:type:: events::mouse_wheel

    A mouse scrolls the wheel on a widget.

.. cpp:type:: events::mouse_drop

    A mouse release over a window that is registered as recipient of drag and
    drop.

.. cpp:type:: events::sizing

    A widget's size is sizing. In this event, A widget's size can be overrided
    with a new size.

.. cpp:type:: events::size

    A widget's size has changed.

.. cpp:type:: events::unload

    A form is closed by clicking the X button, only works for root widget.

.. cpp:type:: events::destroy

    A widget is about to be destroyed.

.. cpp:type:: events::focus

    A widget's focus is changed.

.. cpp:type:: events::key_down

    A keyboard is pressed on a focus widget.

.. cpp:type:: events::key_char

    The focus widget received a character.

.. cpp:type:: events::key_up

    A keyboard is released on a focus widget.

.. cpp:type:: events::shortkey

    The widgets received a shortkey message.

.. cpp:type:: events::elapse

    A widget received a tick that is sended by timer.

The user-defined event function may have a parameter that type is
*const nana::gui::eventinfo&* for quering the event information, such as mouse
position.

.. code-block:: cpp

    void foo();

    void foo_with_parameter(const nana::gui::eventinfo&);

    class user_def_functor
    {
    public:
        void operator()(const nana::gui::eventinfo&);  //user-defined function must have the parameter.
    };

    nana::gui::button().make_event<nana::gui::events::click>(foo);
    nana::gui::button().make_event<nana::gui::events::click>(foo_with_parameter);
    nana::gui::button().make_event<nana::gui::events::click>(user_def_functor());

*make_event* returns a handle for uninstalling the associated user-defined event function and Nana.GUI destroyes the user-defined event function automatically
when the widget is beginning to destroy.

This just describes these general-purpose events, but some widgets like
*nana::gui::treebox* provides some high-level events, such as expanding a node,
these details are only described in the reference of corresponding widget.

See Also
--------

None.
