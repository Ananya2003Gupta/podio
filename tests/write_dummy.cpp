// STL
#include <iostream>
#include <vector>
#include <map>
#include <typeinfo>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"

// HDF5 specific includes
#include "H5Cpp.h"


#include "H5EventInfoCollection.h"
#include "H5ExampleMCCollection.h"
#include "H5ExampleHitCollection.h"
///*
#include "H5ExampleClusterCollection.h"
#include "H5ExampleReferencingTypeCollection.h"
#include "H5ExampleWithComponentCollection.h"
//#include "H5ExampleWithOneRelationCollection.h"
#include "H5ExampleWithVectorMemberCollection.h"
#include "H5ExampleWithNamespaceCollection.h"
#include "H5ExampleWithARelationCollection.h"
#include "H5ExampleWithARelationCollection.h"
//*/

#include "H5ExampleWithStringCollection.h"
#include "H5ExampleWithArrayCollection.h"

using namespace H5;
using namespace std;
using namespace podio;

const H5std_string FILE_NAME("dummy.h5");

int main()
{

	try{

		// create EventStore
		auto store = EventStore();

		// declare HDF5Writer
		auto writer = HDF5Writer(FILE_NAME, &store);


		auto& info       = store.create<EventInfoCollection>("info");
		auto& mcps       = store.create<ExampleMCCollection>("mcparticles");

		/*
		auto& hits       = store.create<ExampleHitCollection>("hits");
		auto& clusters   = store.create<ExampleClusterCollection>("clusters");
		auto& refs       = store.create<ExampleReferencingTypeCollection>("refs");
		auto& refs2      = store.create<ExampleReferencingTypeCollection>("refs2");
		auto& comps      = store.create<ExampleWithComponentCollection>("Component");
		//auto& oneRels    = store.create<ExampleWithOneRelationCollection>("OneRelation");
		auto& vecs       = store.create<ExampleWithVectorMemberCollection>("WithVectorMember");
		auto& namesps    = store.create<ex::ExampleWithNamespaceCollection>("WithNamespaceMember");
		auto& namesprels = store.create<ex::ExampleWithARelationCollection>("WithNamespaceRelation");
		auto& cpytest    = store.create<ex::ExampleWithARelationCollection>("WithNamespaceRelationCopy");
		auto& strings    = store.create<ExampleWithStringCollection>("strings");
		auto& arrays     = store.create<ExampleWithArrayCollection>("arrays");
		*/

		writer.registerForWrite<H5EventInfoCollection>("info");
	    writer.registerForWrite<H5ExampleMCCollection>("mcparticles");

		/*
	    writer.registerForWrite<H5ExampleHitCollection>("hits");
		writer.registerForWrite<H5ExampleClusterCollection>("clusters");
	    writer.registerForWrite<H5ExampleReferencingTypeCollection>("refs");
	    writer.registerForWrite<H5ExampleReferencingTypeCollection>("refs2");
	    writer.registerForWrite<H5ExampleWithComponentCollection>("Component");
	    //writer.registerForWrite<H5ExampleWithOneRelationCollection>("OneRelation");
	    writer.registerForWrite<H5ExampleWithVectorMemberCollection>("WithVectorMember");
	    writer.registerForWrite<H5ExampleWithNamespaceCollection>("WithNamespaceMember");
	    writer.registerForWrite<H5ExampleWithARelationCollection>("WithNamespaceRelation");
	    writer.registerForWrite<H5ExampleWithARelationCollection>("WithNamespaceRelationCopy");
		writer.registerForWrite<H5ExampleWithStringCollection>("strings");
		writer.registerForWrite<H5ExampleWithArrayCollection>("arrays");
		*/

		unsigned nevents = 2;

		for(unsigned i=0; i<nevents; ++i)
		{
			std::cout << "processing event " << i << std::endl;

			auto item1 = EventInfo();
			item1.Number(i);
			info.push_back(item1);

			// ---- add some MC particles ----
			auto mcp0 = ExampleMC();
			auto mcp1 = ExampleMC();
			auto mcp2 = ExampleMC();
			auto mcp3 = ExampleMC();
			auto mcp4 = ExampleMC();
			auto mcp5 = ExampleMC();
			auto mcp6 = ExampleMC();
			auto mcp7 = ExampleMC();
			auto mcp8 = ExampleMC();
			auto mcp9 = ExampleMC();

			mcps.push_back( mcp0 ) ;
			mcps.push_back( mcp1 ) ;
			mcps.push_back( mcp2 ) ;
			mcps.push_back( mcp3 ) ;
			mcps.push_back( mcp4 ) ;
			mcps.push_back( mcp5 ) ;
			mcps.push_back( mcp6 ) ;
			mcps.push_back( mcp7 ) ;
			mcps.push_back( mcp8 ) ;
			mcps.push_back( mcp9 ) ;

			// --- add some daughter relations
			auto p = ExampleMC();
			auto d = ExampleMC();

			p = mcps[0] ;
			p.adddaughters( mcps[2] ) ;
			p.adddaughters( mcps[3] ) ;
			p.adddaughters( mcps[4] ) ;
			p.adddaughters( mcps[5] ) ;
			p = mcps[1] ;
			p.adddaughters( mcps[2] ) ;
			p.adddaughters( mcps[3] ) ;
			p.adddaughters( mcps[4] ) ;
			p.adddaughters( mcps[5] ) ;
			p = mcps[2] ;
			p.adddaughters( mcps[6] ) ;
			p.adddaughters( mcps[7] ) ;
			p.adddaughters( mcps[8] ) ;
			p.adddaughters( mcps[9] ) ;
			p = mcps[3] ;
			p.adddaughters( mcps[6] ) ;
			p.adddaughters( mcps[7] ) ;
			p.adddaughters( mcps[8] ) ;
			p.adddaughters( mcps[9] ) ;

			//--- now fix the parent relations
			for( unsigned j=0,N=mcps.size();j<N;++j){
			p = mcps[j] ;
			for(auto it = p.daughters_begin(), end = p.daughters_end() ; it!=end ; ++it ){
			int dIndex = it->getObjectID().index ;
			d = mcps[ dIndex ] ;
			d.addparents( p ) ;
			}
			}



			writer.writeEvent();
			store.clearCollections();

		}


	} // end of try block


	// catch failure caused by the H5File operations
	catch( FileIException error )
	{
		error.printErrorStack();
		return -1;
	}

	// catch failure caused by the DataSet operations
	catch( DataSetIException error )
	{
		error.printErrorStack();
		return -1;
	}
	// catch failure caused by the DataSpace operations
	catch( DataSpaceIException error )
	{
		error.printErrorStack();
		return -1;
	}
	catch(...)
	{
		std::cout<<"Something terrible happened!"<<std::endl;
		return -1;
	}


	return 0;
}
