HingX Readme
------------

HingX (Health Ingenuity Exchange) is a not for profit organization that
specializes in facilitating the exchange of health information, tools,
and platforms.

The HingX platform (HingX.org) provides a database of free health
information resources.

This database can be accessed remotely using the HingX API
(http://hingx.org:8080/svn/main/HingX/HingX%20API.pdf).

The HingX module defines an API that can be used to search and retrieve
resources from the HingX resource database. The module also defines a
search handler named HingX that can be used to search the resource
database remotely.

Components
----------

The HingX module defines the following components:

  * Drush Commands
    * hingx-search
      Executes a search query against the HingX resource database.
    * hingx-retrieve
      Retrieves the HingX resource that has the given id.
  * Search Handlers
    * HingX
      Executes search queries against the HingX site remotely.

References
----------

1.
HingX website
http://hingx.org/
The HingX website. Contains information about HingX, a search interface
for its resource database, forums, etc.

2.
HingX API
http://hingx.org:8080/svn/main/HingX/HingX%20API.pdf
Describes the HingX API.
