#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Thing.h"
#include "ThingFactory.h"
#include "Python_API.h"

#include <server/WorldRouter.h>

#include <common/const.h>

#include "Character.h"
#include "Creator.h"

using Atlas::Message::Object;

void init_python_api();

ThingFactory thing_factory;

ThingFactory::ThingFactory()
{
    init_python_api();
}

void ThingFactory::readRuleset(const string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

Thing * ThingFactory::newThing(const string & type,const Object & ent, Routing * svr)
{
    if (!ent.IsMap()) {
         cout << "Entity is not a map" << endl << flush;
    }
    Object::MapType entmap = ent.AsMap();
    Thing * thing;
    string py_package;
    if (type.size() == 0) {
        thing = new Thing();
    } else if (type == "creator") {
        thing = new Creator();
    } else if (global_conf->findItem("characters", type)) {
        thing = new Character();
        py_package = global_conf->getItem("characters", type);
    } else if (global_conf->findItem("things", type)) {
        thing = new Thing();
        py_package = global_conf->getItem("things", type);
    } else {
        thing = new Thing();
    }
    cout << "[" << type << " " << thing->name << "]" << endl << flush;
    thing->type = type;
    // Get location from entity, if it is present
    if (entmap.find("loc") != entmap.end()) {
        try {
            string & parent_id = entmap["loc"].AsString();
            BaseEntity * parent_obj = svr->fobjects[parent_id];
            Vector3D pos(0, 0, 0);
            Vector3D velocity(0, 0, 0);
            Vector3D face(1, 0, 0);
            if (entmap.find("pos") != entmap.end()) {
                pos = Vector3D(entmap["pos"].AsList());
            }
            if (entmap.find("velocity") != entmap.end()) {
                velocity = Vector3D(entmap["velocity"].AsList());
            }
            if (entmap.find("face") != entmap.end()) {
                face = Vector3D(entmap["face"].AsList());
            }
            Location thing_loc(parent_obj, pos, velocity, face);
            thing->location = thing_loc;
        }
        catch (Message::WrongTypeException) {
            cout << "ERROR: Create operation has bad location" << endl << flush;
        }
    }
    // Sort out python object
    if (py_package.size() != 0) {
        Create_PyThing(thing, py_package, type);
    }
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    } else {
        cout << "Got no name" << endl << flush;
    }
    thing->merge(entmap);
    return(thing);
}
