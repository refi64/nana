#include <../temp/EditableForm.hpp>
#include <../temp/Numer.hpp>

#include <iostream>    // temp, for debugging
#include <fstream>     // temp, for debugging

void PrintConv(double val, const CUnit::unit_name& from, const CUnit::unit_name& to)
{
    std::cout<< "\n" <<"Converting "<< from << " into " << to ;
    CUnit f_t(from,to);
    if (f_t.error) 
        return; 
    std::cout<< ": " << val << " " << from << " = " << f_t.conv(val)<< " "  << to;
}
void PrintConv(double val, const CUnit& u)
{
    std::cout<< "\n" <<"Converting "<< u.name << " into " << u.base ;
    if (u.error) 
        return; 
    std::cout<< ": " << val << " " << u.name << " = " << u.conv(val)<< " "  << u.base ;
}
void TestCUnits()
{
    for (auto& mag: CUnit::MagnitudesDic())
    {
        std::cout<< "\n\nMagnitude: "<< mag.first;
        for ( auto un: mag.second)
            std::cout<< "\n \t" << CUnit::UnitsDic().at(un);

    }
            std::cout<< "\n" ;
    

    PrintConv(10, "min", "s");
    PrintConv(30, "min", "h");
    PrintConv(1.0/24/60, "day", "s");
    PrintConv(10, "year", "week");
    PrintConv(.1, "kg", "g");
    PrintConv(10, "h", "s");
    PrintConv(10, "dm3", "m3");
    PrintConv(10, "dm", "m");
    PrintConv(10, "mm", "m");
    PrintConv(0.001, "km", "mm");
    PrintConv(50, "°C", "K");
    PrintConv(0, "K", "°C");
    PrintConv(0, "°C", "K");
    PrintConv(0, "°C", "°F");
    PrintConv(100, "°C", "K");
    PrintConv(100, "°C", "°F");
    PrintConv(0.1, "tone", "kg");
    PrintConv(0.1, "amol", "cop");
    PrintConv(100, "cop/µL", "fM");
    PrintConv(1, "g/mL", "g/L");
    PrintConv(1, "g/mL", "M");
    //PrintConv(10, "mL", "s");
    //PrintConv(1000, "dm", "Km");
    //PrintConv(0, "grC", "K");
    //PrintConv(0, "K", "grC");
    //PrintConv(0, "K", "grC");

    std::cout<< "\n";
    Relation Job_d_h("day",8,"h"), Job_w_d("week",5,"day");

    std::cout<< "\n Working days / year: " << Job_w_d("year","day");
    std::cout<< "\n Working days / month: " << Job_w_d("year","day").conv(1.0/12);
    std::cout<< "\n Working h / week: " << Job_d_h.conv( Job_w_d.conv(1) );
    std::cout<< "\n Working h / month: " << Job_d_h.conv( Job_w_d("year","day").conv(1.0/12) );
    std::cout<< "\n Working h / year: " << Job_d_h.conv( Job_w_d("year","day").conv(1) );


    std::cout<< "\n Job: " << Job_d_h;
    std::cout<< "\n Job: " << Job_d_h("week","h");
    PrintConv(5, Job_d_h("day","h"));

    std::cout<< "\n Job: " << Job_w_d("year","week");
    std::cout<< "\n Job: " << Job_w_d("year","day");

    std::cout<< "\n";

    Relation Light("s",300000,"km");
    std::cout<< "\n Light: " << Light;
    std::cout<< "\n Light: " << Light("year","km");

    std::cout<< "\n";
    Relation H2O("mol",18,"g");
    std::cout<< "\n H2O: " << H2O;

    std::cout<< "\n";
    MW NaCl(58.44);
    std::cout<< "\n NaCl: " << NaCl("mol","g");
    std::cout<< "\n NaCl: " << NaCl("g/L","mM");
    std::cout<< "\n NaCl: " ;
    PrintConv(8, NaCl("g/L","mM"));


    std::cout<< "\n";
    MW O2(2*8);
    std::cout<< "\n O2: " << O2("mol","g");
    std::cout<< "\n O2: " << O2("g/L","mM");
    std::cout<< "\n O2: " ;
    PrintConv(8, O2("g/L","mM"));

}



/// A reaction to the ReSize event. Write the new width at position 10 of the caption
void ReSize(const nana::arg_resized &e, nana::widget& w)
{ 
    w.caption(   w.caption().substr(0,10) + std::to_wstring(w.size().width )    ); 
}

class DemoForm : public nana::form, public EditableForm 
{ public:
    OpenSaveBox         osb {*this, STR("Project:" )} , 
                        osb2{*this, STR("Project2:")} ;
    nana::button	but1{*this, STR("1-free   : ")} , 
                        but2{*this, STR("2-Min 200: ")} , 
                        but3{*this, STR("3-Fix 200: ")} , 
                        but4{*this, STR("4-200-600: ")} ;
    nana::label   _label{*this, STR("Num out:")} ;
    nana::NumerUpDown num1 {*this, STR("G:") , -5, -30, 30}, 
                           num2 {*this, STR("Tm:"), 60,  30, 90};
    nana::textbox _num{*this, } ;
    nana::button  _up{*this, STR("^")} , _down{*this, STR("v")} ;
    nana::UnitPicker      UPicker{*this, "m"} ;
    nana::NumUnitUpDown   Ta{*this, STR("Ta:"), 55,  50, 60, "°C" } ; 
                //Ta           ( *this, STR("Conc:"), 0.1,  0, 20, "M", STR(""), 0.1,3 /*, DefLayFile , step=1, decimals=2*/)

    DemoForm ():nana::form (nana::rectangle( nana::point(800,210), nana::size(500,250) )),
                EditableForm    (nullptr, *this, STR("Configurable Window"), STR("Layot_Demo.lay.txt"))
    {
        _label.text_align(nana::align::right  ); 
        Ta.ResetLayout     (10,40,50 );

         osb.add_filter (STR("Text File" ), STR("*.txt;*.doc"));
	     osb.add_filter (STR("All File"  ), STR("*.*"));
	    osb2.add_filter (STR("Text File" ), STR("*.txt"));
	    osb2.add_filter (STR("Todos File"), STR("*.*"));

        InitMyLayout();
        AddMenuProgram();
        SelectClickableWidget( _menuBar);
        SelectClickableWidget( but2);

        but1.events().resized ([&](const nana::arg_resized &e){ReSize(e, but1);});
	    but2.events().resized ([&](const nana::arg_resized &e){ReSize(e, but2);});
	    but3.events().resized ([&](const nana::arg_resized &e){ReSize(e, but3);});
	    but4.events().resized ([&](const nana::arg_resized &e){ReSize(e, but4);});

    }
 
    void SetDefLayout   () override
    {
        _DefLayout= "vertical      gap=2             \n\t"
	                 "       <weight=25>                \n\t"
	                 "       <Project weight=23>       \n\t "
	                 "       <gap=2 min=30 <b1> <b2 min=200> <b3 weight=200> >   \n\t "
	                 "       <<b4 min=200 max=600>min=30 <weight=6>>           \n\t "
	                 "       <  weight=22 <label weight=60 ><Num> <vertical weight=50 <UpDown> >>     \n\t   "
	                 "       <  weight=23 <Ta  weight= 200><> <num  weight=280 ><>  <Unit weight=50 gap=2 >>         \n\t  "
	                 "       <Project2 weight=23>       "
                  ;
    }
    void AsignWidgetToFields() override
    {
	    _place.field("Project" )<<osb;
	    _place.field("Project2")<<osb2;
	    _place.field("b1"     ) <<but1;
	    _place.field("b2"     ) <<but2;
	    _place.field("b3"     ) <<but3;
	    _place.field("b4"     ) <<but4;
	    _place.field("label"  ) << _label;
	    _place.field("Num"    ) << _num  ;
	    _place.field("UpDown" ) << _up << _down  ;
	    _place.field("Ta"     ) << Ta;
	    _place.field("num"    ) <<num1 << num2;
	    _place.field("Unit"   ) << UPicker/*._cb*/ ;
    }

 };

//int main_temp ()
int main()
{
    TestCUnits();
	DemoForm form;
	form.show();
    try {
	        nana::exec();

        } catch (std::exception e)
        {
            std::cout<< std::endl<< e.what();
            throw ;
        } catch (...)
        {
            std::cout<< std::endl<< "exeption !!";
            throw ;
        }
	return 0;
}


