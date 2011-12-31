#include <RTI.hh>
#include <fedtime.hh>
#include <NullFederateAmbassador.hh>

#include "test_Inheritance_cmdline.h"

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#include <algorithm>
#include <process.h>
#define	sleep(a)	      Sleep(a * 1000)
#endif

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::auto_ptr;
/**
 * The InheritanceFederate inherit from the classical
 * NullFederateAmbassador.
 */
class InheritanceFederate : public NullFederateAmbassador {

private:
    string federationName;
    string federationFile;

    RTI::ObjectClassHandle        objectClassId;
    RTI::AttributeHandle          attributeId;
    RTI::InteractionClassHandle   interactionClassId;
    RTI::ParameterHandle          parameterId;
    bool                          verbose;

    void showRTIException(RTI::Exception& e, std::ostream& os, bool withEndl) {
        os << "RTI::Exception<" << e._name <<"> ["
             << (e._reason ? e._reason : "undefined") << "]";
        if (withEndl) {
            os << std::endl;
        }
    }

public:
    InheritanceFederate(std::string federationFile, std::string federationName, bool verbose=false) {
        this->verbose        = verbose;
        if (verbose) {
            cout << "federationFile = " << federationFile <<endl;
            cout << "federationName = " << federationName <<endl;
        }
        this->federationFile = federationFile;
        this->federationName = federationName;
        this->objectClassId      = 0;
        this->attributeId        = 0;
        this->interactionClassId = 0;
        this->parameterId        = 0;
    };

    virtual ~InheritanceFederate() throw (RTI::FederateInternalError) {
    };

    bool
    create() {
        bool retval = false;

        try {
            myRTIamb.createFederationExecution(federationName.c_str(),
                    federationFile.c_str());
            if (verbose) {
                cout << "createFederationExecution SUCCESS." << endl;
            }
            retval = true;
        } catch ( RTI::FederationExecutionAlreadyExists& e) {
            cout << "--> Federation already created [probably] by another federate." << endl;
        } catch ( RTI::Exception &e ) {
            cerr << "RTI exception: " << e._name << " ["
                    << (e._reason ? e._reason : "undefined") << "]." << endl;
        } catch ( ... ) {
            cerr << "Error: Unknown non-RTI exception." << endl;
        }

        return retval;
    } /* end of create */

    bool
    destroy() {
        bool retval = false;
        try {
            myRTIamb.destroyFederationExecution(federationName.c_str());
            retval = true;
        } catch (RTI::FederatesCurrentlyJoined &e) {
            cout << "Federates currently joined." << endl;
        } catch ( RTI::Exception &e ) {
            cerr << "RTI exception: " << e._name << " ["
                    << (e._reason ? e._reason : "undefined") << "]." << endl;
        } catch ( ... ) {
            cerr << "Error: Unknown non-RTI exception." << endl;
        }
        return retval;
    } /* end of destroy */

    bool
    join(std::string federateName) {
        bool retval = false;
        try {
            if (verbose) {
                cout << "Try to join federation as <" << federateName << ">...";
            }
            myRTIamb.joinFederationExecution(federateName.c_str(),
                    federationName.c_str(),
                    this);
            if (verbose) {
                cout << "SUCCESS." << endl;
            }
            retval = true;
        } catch (RTI::FederateAlreadyExecutionMember& e) {
            cerr << "Federate already execution member" << endl;
        } catch ( RTI::Exception &e ) {
            cerr << "RTI exception: " << e._name << " ["
                    << (e._reason ? e._reason : "undefined") << "]." << endl;
        } catch ( ... ) {
            cerr << "Error: Unknown non-RTI exception." << endl;
        }
        return retval;
    } /* end of join */

    bool
    resign() {
        bool retval = false;
        try {
            if (verbose) {
                cout << "Try to resign from federation...";
            }
            myRTIamb.resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
            if (verbose) {
                cout << "SUCCESS." << endl;
            }
            retval = true;
        } catch ( RTI::Exception &e ) {
            cerr << "RTI exception: " << e._name << " ["
                    << (e._reason ? e._reason : "undefined") << "]." << endl;
        } catch ( ... ) {
            cerr << "Error: Unknown non-RTI exception." << endl;
        }
        return retval;
    } /* end of resign */

    bool
    getObjectHandles(const std::string& className, const std::string& attributeName) {
        bool retval = true;

        if (verbose) {
            cout << "Getting object handles..." <<endl;
        }
        try {
            objectClassId = myRTIamb.getObjectClassHandle(className.c_str());
            cout << "   " << className << " ID = " << objectClassId << endl;
        } catch (RTI::Exception& e) {
            cerr << "   Unable to obtain class handle for object class <"<< className <<">" <<endl;
            retval = false;
        }

        if (attributeName.length()>0) {
            try {
                attributeId = myRTIamb.getAttributeHandle(attributeName.c_str(),objectClassId);
                cout << "   " << attributeName << " ID = " << attributeId << endl;
            } catch (RTI::Exception& e){
                cerr << "   Unable to obtain attribute handle for object class attribute <"<< attributeName<<">" <<endl;
                retval = false;
            }
        }
        return retval;
    } /* end of getClassHandles */

    bool
    getInteractionHandles(std::string className, std::string parameterName) {
        bool retval = true;

        if (verbose) {
            cout << "Getting interaction handles..." <<endl;
        }
        try {
            interactionClassId = myRTIamb.getInteractionClassHandle(className.c_str());
            cout << "   " << className << " ID = " << interactionClassId << endl;
        } catch (RTI::Exception& e) {
            cerr << "   Unable to obtain class handle for interaction class <"<< className <<">" <<endl;
            retval = false;
        }

        if (parameterName.length()>0) {
            try {
                parameterId = myRTIamb.getParameterHandle(parameterName.c_str(),interactionClassId);
                cout << "   " << parameterName << " ID = " << parameterId << endl;
            } catch (RTI::Exception& e){
                cerr << "   Unable to obtain attribute handle for interaction class parameter <"<< parameterName<<">" <<endl;
                retval = false;
            }
        }
        return retval;
    } /* end of getInteractionHandles */

    bool
    publish() {
        bool retval = false;
        if (interactionClassId>0) {
            try {
                myRTIamb.publishInteractionClass(interactionClassId);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to publishInteractionClass:";
                showRTIException(e,cerr,true);
            }
        }
        if ((objectClassId>0) & (attributeId>0)) {
            try {
                auto_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(1));
                attributes->add(attributeId);
                myRTIamb.publishObjectClass(objectClassId,*attributes);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to publishObjectClass:";
                showRTIException(e,cerr,true);
                retval = false;
            }
        }
        return retval;
    } /* end of publish */

    bool
    unpublish() {
        bool retval = false;
        if (interactionClassId>0) {
            try {
                myRTIamb.unpublishInteractionClass(interactionClassId);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to unpublishInteractionClass" << endl;
            }
        }
        if ((objectClassId>0) & (attributeId>0)) {
            try {
                myRTIamb.unpublishObjectClass(objectClassId);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to unpublishObjectClass" << endl;
                retval = false;
            }
        }
        return retval;
    } /* end of unpublish */

    void UAV() {

    } /* end of UAV */

    void SI() {
        auto_ptr<RTI::ParameterHandleValuePairSet> parameters(RTI::ParameterSetFactory::create(1));
        /* parameters->add(parameterId,; */
        try {
            myRTIamb.sendInteraction(interactionClassId,*parameters,"");
            if (verbose) {
                cout << "SI<" << interactionClassId << ">" << endl;
            }
        } catch (RTI::Exception& e) {
            cerr << "Unable to sendInteraction:";
            showRTIException(e,cerr,true);
        }
    } /* end of SI */

    bool
    subscribe() {
        bool retval = false;
        if (objectClassId>0) {
            // Add attribute handle to the attribute set
            // Before, we create the Set with one attribute
            auto_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(1));
            attributes->add(attributeId);
            try {
                myRTIamb.subscribeObjectClassAttributes(objectClassId,*attributes);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to subscribeObjectClassAttributes:";
                showRTIException(e,cerr,true);
            }
        }
        if (interactionClassId>0) {
            try {
                myRTIamb.subscribeInteractionClass(interactionClassId);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to subscribeInteractionClass:";
                showRTIException(e,cerr,true);
                retval = false;
            }
        }
        return retval;
    } /* end of subscribe */

    bool
    unsubscribe() {
        bool retval = true;
        if (objectClassId>0) {
            try {
                myRTIamb.unsubscribeObjectClass(objectClassId);
            } catch (RTI::Exception& e) {
                cerr << "Unable to unsubscribeObjectClass:";
                showRTIException(e,cerr,true);
                retval = false;
            }
        }
        if (interactionClassId>0) {
            try {
                myRTIamb.unsubscribeInteractionClass(interactionClassId);
                retval = true;
            } catch (RTI::Exception& e) {
                cerr << "Unable to unsubscribeInteractionClass:";
                showRTIException(e,cerr,true);
                retval = false;
            }
        }
        return retval;
    } /* end of unsubscribe */

    void
    tickRTI() {
        try {
            myRTIamb.tick();
            if (verbose) {
                cout << "tick()."<<endl;
            }
        } catch (RTI::Exception& e) {
            showRTIException(e,cerr,true);
        } catch ( ... ) {
            cerr << "Error: unknown non-RTI exception." << endl;
        }
    } /* end of tickRTI() */

    void
    tickRTI(double min, double max) {
        try {
            myRTIamb.tick(min, max);
            if (verbose) {
                cout << "tick("<< min <<","<<max<<")."<<endl;
            }
         } catch (RTI::Exception &e) {
            showRTIException(e,cerr,true);
        } catch ( ... ) {
            cerr << "Error: unknown non-RTI exception." << endl;
        }
    } /* end of tickRTI(double,double) */

    virtual void reflectAttributeValues(RTI::ObjectHandle oh, const RTI::AttributeHandleValuePairSet &,
                    const RTI::FedTime &t, const char *, RTI::EventRetractionHandle)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateOwnsAttributes,
           RTI::InvalidFederationTime, RTI::FederateInternalError) {
        RTI::FedTime* ft = RTI::FedTimeFactory::makeZero();
        *ft = t;
        char cbuf[ft->getPrintableLength()];
        ft->getPrintableString(&(cbuf[0]));
        std::cout << "RAV(t=" << cbuf << ", OH="<< oh << ")" <<std::endl;
    }

    virtual void reflectAttributeValues(RTI::ObjectHandle oh, const RTI::AttributeHandleValuePairSet &, const char *)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateOwnsAttributes,
           RTI::FederateInternalError) {
        std::cout << "RAV(OH="<< oh << ")" <<std::endl;;
    }

    virtual void receiveInteraction(RTI::InteractionClassHandle ich, const RTI::ParameterHandleValuePairSet &,
                    const RTI::FedTime &t, const char *, RTI::EventRetractionHandle)
    throw (RTI::InteractionClassNotKnown, RTI::InteractionParameterNotKnown, RTI::InvalidFederationTime,
           RTI::FederateInternalError) {
        RTI::FedTime* ft = RTI::FedTimeFactory::makeZero();
        *ft = t;
        char cbuf[ft->getPrintableLength()];
        ft->getPrintableString(&(cbuf[0]));
        std::cout << "RI(t=" << cbuf << ", ICH="<< ich << ")" <<std::endl;
    }

    virtual void receiveInteraction(RTI::InteractionClassHandle ich, const RTI::ParameterHandleValuePairSet &,
                    const char *)
    throw (RTI::InteractionClassNotKnown, RTI::InteractionParameterNotKnown, RTI::FederateInternalError) {
        std::cout << "RI(ICH="<< ich << ")" <<std::endl;
    }
protected:

    RTI::RTIambassador myRTIamb;
};

int
main(int argc, char **argv) {

    bool status = true;
    // Command line arguments handling (generated with gengetopt)
    gengetopt_args_info args ;
    if (cmdline_parser(argc, argv, &args))
        exit(EXIT_FAILURE);

    InheritanceFederate InheritanceFederate(std::string(args.fedfile_arg),
            std::string(args.fedname_arg),args.verbose_flag);

    status &= InheritanceFederate.create();

    if (args.joinname_given) {
        status &= InheritanceFederate.join(std::string(args.joinname_arg));
    }

    if (args.joinname_given && args.objectclass_given) {
        if (args.attribute_given) {
            status &= InheritanceFederate.getObjectHandles(std::string(args.objectclass_arg),
                    std::string(args.attribute_arg));
        } else {
            status &= InheritanceFederate.getObjectHandles(std::string(args.objectclass_arg),
                    std::string(""));
        }
    }

    if (args.joinname_given && args.interaction_given) {
        if (args.parameter_given) {
            status &= InheritanceFederate.getInteractionHandles(std::string(args.interaction_arg),
                    std::string(args.parameter_arg));
        } else {
            status &= InheritanceFederate.getInteractionHandles(std::string(args.interaction_arg),
                    std::string(""));
        }
    }

    /* subscribe or publish */
    if (!args.subscribe_flag) {
        cout << args.fedname_arg << " is a PUBLISHER" << endl;
        InheritanceFederate.publish();
    } else {
        cout << args.fedname_arg << " is a SUBSCRIBER" << endl;
        InheritanceFederate.subscribe();
    }

    /* main simulation loop */
    for (int i=0; i< args.time_arg; ++i) {
        if (!args.subscribe_flag) {
           InheritanceFederate.UAV();
           InheritanceFederate.SI();
        }
        InheritanceFederate.tickRTI(1,2);
    }

    /* un-subscribe or un-publish */
    if (!args.subscribe_flag) {
        InheritanceFederate.unpublish();
    } else {
        InheritanceFederate.unsubscribe();
    }

    if (args.joinname_given) {
        status &= InheritanceFederate.resign();
    }

    status &= InheritanceFederate.destroy();
    if (status) {
        cout << "GLOBAL SUCCESS." <<endl;
    } else {
        cout << "GLOBAL ERROR." <<endl;
    }
	return status ? 0 : 1;
}
