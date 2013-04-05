Provides the backoffice functionality present in Commerce Kickstart v2.

Contains three submodules: commerce_backoffice_product, commerce_backoffice_order, commerce_backoffice_content.

Commerce Backoffice Product
---------------------------
Provides a better experience for stores using nodes as product displays for grouping product variations (commerce_product entities).

    The node/add screen is now split into two tabs, "Create content" (ordinary node types), and "Create product" (product display node types).
    The products view is now a view of nodes, showing product displays. Contains special exposed filters for filtering by product display type, and product display categories.
    Uses the megarow pattern to provide a "Quick Edit" link in the view, that shows all product variations for that product display, right underneath the triggering row. The status and price can be modified directly.
    Modifies the "Content types" screen by adding additional help text for understanding product displays, and adds a column to the table that indicates whether the node type is a product display node type*.
    Disables the Commerce-provided "Product types" UI, and provides a custom "Product variation types" UI that contains additional help text, hides fields that are not used by the Inline Entity Form (help, description for each type), and provides the ability to create a matching node type for each created product variation type.

* - Every node type with a product reference field is considered a product display node type.

Dependencies: Inline Entity Form, Views Megarow, Views Bulk Operations

Commerce Backoffice Order
-------------------------
Provides a better order management experience.

    The order view has been redesigned for better usability, and contains exposed filters for the order status and creation date.
    Provides rules-powered bulk operations for modifying the order statuses.
    Uses the megarow pattern to provide a "Quick Edit" link in the view, that shows the line items, customer information, payments, recent messages (if Commerce Message is enabled). Allows the admin to add a new message or change the order status.

Dependencies: Date, EVA, Views Megarow, Views Bulk Operations

Commerce Backoffice Content
---------------------------
Provides views for managing content (excluding all product display types, and their categories) and comments.
