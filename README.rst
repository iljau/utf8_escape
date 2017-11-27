utf8_escape
-----------------------------

HTML escape for utf8-encoded strings

.. code-block:: python

  # coding=utf8
  from utf8_escape import escape_html
  print escape_html("меньше < more")

.. code-block:: python

  меньше &lt; more
