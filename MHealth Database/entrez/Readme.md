ENTREZ PACKAGE README

===== BACKGROUND: =====

The Entrez Package define an API that allows Drupal sites to
communicate with Entrez databases using E-utilities.

E-utilities is a collection of CGI scripts that perform queries against
Entrez databases, such as PubMed, and returns results in a variety of
formats, including XML.

Entrez itself is a database search system that allows users to search
several separate databases at National Center for Biotechnology
Information (NCBI), which is maintained by the National Institutes of
Health (NIH).

The Entrez database collection includes several popular databases such
as PubMed which draws content primarily from the MEDLINE database.

The Entrez Package contains several modules. The core module is the
Entrez module, which defines the basic API for accessing Entrez using
the E-utilities.

The Package also contains specialized modules for accessing specific
databases. For example, the PubMed module defines functions for
importing content from the PubMed database.

===== COMPONENTS: =====

This module defines the following components:

  * Drush Commands
      * entrez-search
        Executes a search query against an Entrez database and returns the result as an XML document.
      * entrez-fetch
        Retrieves the Entrez database records that have the given ids.
      * entrez-links
        Retrieves the links associated with the given Entrez database records.

===== REFERENCES: ===== 

1.
MEDLINE®/PubMed® Data Element (Field) Descriptions
http://www.nlm.nih.gov/bsd/mms/medlineelements.html
Provides descriptions of the Pubmed record fields.
