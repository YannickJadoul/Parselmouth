{{ fullname | escape | underline}}

{% set exclude_members = ['__weakref__', '__doc__', '__module__', '__dict__', '__members__', '__getstate__', '__setstate__', '__annotations__'] %}

.. currentmodule:: {{ module }}

.. autoclass:: {{ objname }}
   :show-inheritance:
   :members:
   :inherited-members:
   :undoc-members:
   :special-members:
   :exclude-members: {{ ', '.join(exclude_members) }}

   {% block methods %}
   {% if methods %}
   .. rubric:: {{ _('Methods') }}

   .. autosummary::
      :nosignatures:
   {% for item in methods %}
      {% if item not in exclude_members %}
      ~{{ name }}.{{ item }}
      {% endif %}
   {%- endfor %}
   {% endif %}
   {% endblock %}

   {% block attributes %}
   {% if attributes %}
   .. rubric:: {{ _('Attributes') }}

   .. autosummary::
   {% for item in attributes %}
      {% if item not in exclude_members %}
      ~{{ name }}.{{ item }}
      {% endif %}
   {%- endfor %}
   {% endif %}
   {% endblock %}
