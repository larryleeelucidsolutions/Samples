MHealthlth Database Record DB Readme

The MHealth DB package contains a collection of modules that provide
functionality specific to the MHealth DB site.

This package contains the following modules:

  1. mhealthdb
     Defines the MHealth DB Record node type, the MTERG vocabulary, and
     an API for manipulating these record nodes. 

  2. mhealthdb_commentary
     Provides an article commentary feature.

  3. mhealthdb_import
     Imports records from PubMed using a search query and Cron.

  4. mhealthdb_search
     Defines the MHealth DB advanced search form.

The MHealth DB module defines the MHealth DB Record node type. This
node type is used to represent all of the articles and resources
stored on the MHealth Evidence site. Each resource may have more than
one "source". Currently a source is either a PubMed or Google Scholar
Record node. The API defined in the MHealth DB module includes
functions for syncing MHealth DB Record nodes with these source nodes.
This syncing is largely automatic, but the module also defines Drush
commands that can be used to sync records manually.

Specifically, the module defines the following components:

  * Node Types
    * MHealth Database Record
      mhealthdb_record
  * Vocabularies
    * mHealth Evidence (TERG) Taxonomy
      mhealth_evidence_terg_taxonomy
  * Drush Commands
    * mhealthdb-sync-records
      Iterates over all of the sources and creates/updates/removes
      MHealthDB record nodes as needed to keep them in sync with Pubmed
      and Google Scholar sources.
