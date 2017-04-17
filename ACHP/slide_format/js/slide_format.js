/*
  This script scans the current document for elements that have
  been tagged with the `slide-format-target` element class.

  For every target element, this script creates a slide feature
  element that represents those elements within the target element
  that have been tagged with the `slide-format-target-item` class.

  Once created, this script will insert the created slide feature
  element immediately after the target element.

  This script will initialize the slide feature so that it is
  initially focused on the item element that has been tagged with
  the `slide-format-target-item-active` class.

  This module uses a collection of HTML elements to represent
  slide features. A diagram showing the relationships between these
  elements is given below:

  feature element
  +-------------------------------------------------------+
  | slide container element                               |
  | +---------------------------------------------------+ |
  | |  focus element                                    | |
  | |  +----------------------------------------------+ | |
  | |  |                                              | | |
  | |  +----------------------------------------------+ | |
  | |  slide element                                    | |
  | |  +----------------------------------------------+ | |
  | |  | item elements [] [] []                       | | |
  | |  +----------------------------------------------+ | |
  | +---------------------------------------------------+ |
  | slide left button element  slide right button element |
  | +--+                                             +--+ |
  | |  |                                             |  | |
  | +--+                                             +--+ |
  +-------------------------------------------------------+

  The focus and slide elements must be absolutely positioned within
  the slide container element.

  The focus element defines the region that items items within the
  slide element are aligned with. By default, the slide element
  will align with the left edge of the focus element.

  If the active item would lie outside of the focus element,
  however, the slide element is shifted to the left until the right
  edge of the active item aligns with the right edge of the focus
  element.

  By default, the slide container element is the same width as the
  feature element.

  If the slide element is wider than the feature element, however, the
  slide element extends so that it can encompass the entire width
  of the slide element plus a margin to allow the slide element to
  be dragged so that the slide element's edges align with the feature
  element's edges.
*/

(function ($) {

  /*
    Finds those list elements that have been
    tagged with the `slide-format-target` class,
    creates slide feature elements that represent
    these elements, and inserts these feature
    elements immediately after the target elements.
  */
  $(document).ready (function () {
    getTargetElements ().each (function (i, targetElement) {
      targetElement = $(targetElement);

      var feature = new Feature (targetElement);
      targetElement.after (feature.element);
      feature.init ();
    });
  });
  
  // II. THE FEATURE CLASS

  /*
    Accepts one argument: targetElement, a jQuery
    HTML Element that represents a target element;
    and returns a Feature object that represents
    targetElement.
  */
  function Feature (targetElement) {
    this.index = 0;
    this.itemElements = createItemElements (this, targetElement);
    this.focusElement = createFocusElement ();
    this.slideLeftButtonElement = createSlideLeftButtonElement (this);
    this.slideRightButtonElement = createSlideRightButtonElement (this);
    this.slideElement = createSlideElement (this.itemElements);
    this.slideContainerElement = createSlideContainerElement (
      this.focusElement,
      this.slideElement
    );
    this.element = createFeatureElement (
      this.slideContainerElement,
      this.slideLeftButtonElement,
      this.slideRightButtonElement
    );
  }

  /*
    Accepts no arguments, resizes, positions,
    enables/disables feature elements, and
    returns undefined.
  */
  Feature.prototype.init = function () {
    var self = this;
    this.resizeSlideElement ();
    this.resizeFocusElement ();
    this.resizeSlideContainerElement ();
    this.centerFocusElement ();
    this.centerSlideContainerElement ();
    this.positionSlideElement ();
    this.focus ();
    this.initSlideButtons ();

    $(window).resize (function () {
      self.resizeFocusElement ();
      self.resizeSlideContainerElement ();
      self.centerFocusElement ();
      self.centerSlideContainerElement ();
      self.positionSlideElement ();
      self.focus ();
    });
  }

  /*
    Accepts no arguments, sets the show/hide
    functions for the slide buttons, and returns
    undefined.
  */
  Feature.prototype.initSlideButtons = function () {
    var self = this;
    window.setInterval (function () {
      self.shouldShowSlideLeftButton () ? self.showSlideLeftButton () : self.hideSlideLeftButton ();
      self.shouldShowSlideRightButton () ? self.showSlideRightButton () : self.hideSlideRightButton ();
    }, 1000);
  }

  /*
    Accepts no arguments, resizes the focus
    element, and returns undefined.

    Note: This function scales the focus element
    so that it spans the width of the feature
    element. This may be used in conjunction
    with a max-width property specified within
    a CSS stylesheet.
  */
  Feature.prototype.resizeFocusElement = function () {
    this.focusElement.width (this.element.width ());
  }

  /*
    Accepts no arguments, centers the focus
    element within the slide container element,
    and returns undefined.

    Note: This function assumes that the focus
    element is absolutely positioned within the
    slide container element.
  */
  Feature.prototype.centerFocusElement = function () {
    this.focusElement.css ('left',
      (this.slideContainerElement.width () - this.focusElement.width ()) / 2);
  }

  /*
    Accepts no arguments, resizes the slide
    container element, and returns undefined.

    Note: By default, this function sets the
    slide container element so that it is the
    same width as the feature element. If, however,
    the slide element is wider than the feature
    element, this function extends the slide
    container element so that it can wrap the
    slide element plus a margin so that the
    slide element can be dragged to show all of
    its feature item elements.
  */
  Feature.prototype.resizeSlideContainerElement = function () {
    var slideWidth = this.slideElement.outerWidth (true);
    var focusWidth = this.focusElement.width ();

    return slideWidth < focusWidth ?
      this.slideContainerElement.width (focusWidth):
      this.slideContainerElement.width (2 * slideWidth - focusWidth);
  }

  /*
    Accepts no arguments, centers the slide
    container element within the feature element,
    and returns undefined.

    Note: this function assumes that the slide
    container element is absolutely positioned
    within the feature element.
  */
  Feature.prototype.centerSlideContainerElement = function () {
    this.slideContainerElement.css ('left',
      (this.element.width () - this.slideContainerElement.width ()) / 2);
  }

  /*
    Accepts no arguments, resizes the slide
    element so that can fit all of its item
    elements; and returns undefined.
  */
  Feature.prototype.resizeSlideElement = function () {
    var slideWidth = this.itemElements.reduce (
      function (slideWidth, itemElement) {
        return slideWidth + $(itemElement).outerWidth (true);
      }, 0
    );
    this.slideElement.width (slideWidth + 'px');
  }

  /*
    Accepts no arguments, aligns the slide
    element with the left-hand side of the focus
    element, and returns undefined.
  */
  Feature.prototype.positionSlideElement = function () {
    this.slideElement.css ('left', this.focusElement.position ().left);
  }

  /*
    Accepts no arguments, slides to the currently
    active item and returns undefined.
  */
  Feature.prototype.focus = function () {
    var itemElement = this.getActiveItemElement ();
    itemElement && this.slideTo (getItemElementIndex (itemElement));
  }

  /*
    Accepts one argument: index, an integer
    that references an item element; slides
    the slide element so that the item
    referenced by index is positioned within
    the focus element; and returns undefined.
  */
  Feature.prototype.slideTo = function (index) {
    var itemElement = this.getItemElement (index);
    var itemWidth = itemElement.outerWidth (true);
    var itemLeft  = this.slideElement.position ().left + itemElement.position ().left;
    var itemRight = itemLeft + itemElement.outerWidth (true);
    var focusLeft = this.focusElement.position ().left;
    var focusRight = focusLeft + this.focusElement.width ();

    if (itemLeft < focusLeft) {
      var delta = focusLeft - itemLeft;
      return this.slide (delta);
    } else if (itemRight > focusRight) {
      var delta = Math.max (focusLeft, focusRight - itemWidth) - itemLeft;
      return this.slide (delta);
    }
  }

  /*
    Accepts no arguments, slides the slide
    element to the right, and returns undefined.

    Note: this function assumes that the slide
    element is positioned absolutely relative
    to the slide container element and that both
    are clipped by the feature element.
  */
  Feature.prototype.slideLeft = function () {
    this.slide (this.element.width ());
  }

  /*
    Accepts no arguments, slides the slide
    element to the left, and returns undefined.

    Note: this function assumes that the slide
    element is positioned absolutely relative
    to the slide container element and that both
    are clipped by the feature element.
  */
  Feature.prototype.slideRight = function () {
    this.slide (-this.element.width ());
  }

  /*
    Accepts one argument: delta, an integer
    that represents an offset; slides the slide
    element to the right by delta pixels; and
    returns undefined.

    Note: delta may be a negative value, in this
    case, the slide element slides the slide
    element to the left.

    Note: this function assumes that the slide
    element is positioned absolutely relative
    to the slide container element and that both
    are clipped by the feature element.
  */
  Feature.prototype.slide = function (delta) {
    this.slideElement.animate ({left: this.slideElement.position ().left + this.constrainDelta (delta)});
  }

  /*
    Accepts one argument: delta, an integer
    that represents a pixel offset; and returns
    an integer, delta' that represents a valid
    pixel offset for the slide element.

    Note: this function assumes that the slide
    element is positioned absolutely relative
    to the slide container element and that both
    are clipped by the feature element.
  */
  Feature.prototype.constrainDelta = function (delta) {
    var left        = this.slideElement.position ().left;
    var right       = left + this.slideElement.width ();
    var rightMargin = this.slideContainerElement.width () - right;
    return left < -delta ? -left :
      (rightMargin < delta ? rightMargin : delta);
  }

  /*
    Accepts no arguments and returns a jQuery
    HTML Element that represents the currently
    selected item element.
  */
  Feature.prototype.getSelectedItemElement = function () {
    return this.getItemElement (this.index);
  }

  /*
    Accepts one argument: index, a natural
    number that references an item element;
    and returns a jQuery HTML Element that
    represents the item element referenced
    by index.
  */
  Feature.prototype.getItemElement = function (index) {
    return index < this.itemElements.length ? this.itemElements [index] : null;
  }

  /*
    Accepts no arguments and returns a natural
    number that references the currently active
    item element.
  */
  Feature.prototype.getActiveItemElement = function () {
    return _.find (this.itemElements, itemElementIsActive);
  }

  /*
    Accepts no arguments, shows the slide left
    button, and returns undefined.
  */
  Feature.prototype.showSlideLeftButton = function () {
    this.slideLeftButtonElement.show ();
  }

  /*
    Accepts no arguments, shows the slide right
    button, and returns undefined.
  */
  Feature.prototype.showSlideRightButton = function () {
    this.slideRightButtonElement.show ();
  }

  /*
    Accepts no arguments, hides the slide left
    button, and returns undefined.
  */
  Feature.prototype.hideSlideLeftButton = function () {
    this.slideLeftButtonElement.hide ();
  }

  /*
    Accepts no arguments, hides the slide right
    button, and returns undefined.
  */
  Feature.prototype.hideSlideRightButton = function () {
    this.slideRightButtonElement.hide ();
  }

  /*
    Accepts no arguments and returns true iff
    the slide left button should be displayed.
  */
  Feature.prototype.shouldShowSlideLeftButton = function () {
    if (!this.isSlideContainerCropped ()) { return false; }

    var left = this.slideElement.position ().left;
    var right       = left + this.slideElement.width ();
    var rightMargin = this.slideContainerElement.width () - right;
    return rightMargin > 0;
  }

  /*
    Accepts no arguments and returns true iff
    the slide right button should be displayed.
  */
  Feature.prototype.shouldShowSlideRightButton = function () {
    if (!this.isSlideContainerCropped ()) { return false; }

    return this.slideElement.position ().left > 0;  
  }

  /*
    Accepts no arguments and returns true iff the
    slide container element has been cropped -
    I.E. the slide container element is wider
    than the feature element.
  */
  Feature.prototype.isSlideContainerCropped = function () {
    return this.element.width () < this.slideContainerElement.width ();
  }

  /*
    Accepts three arguments:

    * slideContainerElement, a jQuery HTML
      Element that represents a slide container
      element
    * slideLeftButtonElement, a jQuery HTML
      Element that represents a slide left button
      element
    * and slideRightButtonElement, a jQuery
      HTML Element that represents a slide right
      button element

    and returns a jQuery HTML Element that
    represents a feature element.
  */
  function createFeatureElement (slideContainerElement, slideLeftButtonElement, slideRightButtonElement) {
    return $('<div></div>')
      .addClass (getFeatureElementClassName ())
      .append (slideContainerElement)
      .append (slideLeftButtonElement)
      .append (slideRightButtonElement);
  }

  /*
    Accepts one argument: feature, a Feature
    object; and returns a jQuery HTML Element
    that represents a slide left button element.
  */
  function createSlideLeftButtonElement (feature) {
    return createSlideButtonElement ()
      .addClass (getLeftSlideButtonElementClassName ())
      .click (function () { feature.slideLeft (); });
  }

  /*
    Accepts one argument: feature, a Feature
    object; and returns a jQuery HTML Element
    that represents a slide right button element.
  */
  function createSlideRightButtonElement (feature) {
    return createSlideButtonElement ()
      .addClass (getRightSlideButtonElementClassName ())
      .click (function () { feature.slideRight (); });
  }

  /*
    Accepts no arguments and returns a jQuery
    HTML Element that represents an abstract
    slide button element.
  */
  function createSlideButtonElement () {
    return $('<div></div>').addClass (getSlideButtonElementClassName ());
  }

  /*
    Accepts two arguments:

    * focusElement, a jQuery HTML Element that
      represents a focus element
    * and slideElement, a jQuery HTML Element
      that represents a slide element

    and returns a jQuery HTML Element that
    represents a slide container element.
  */
  function createSlideContainerElement (focusElement, slideElement) {
    return $('<div></div>')
      .addClass (getSlideContainerElementClassName ())
      .append (focusElement)
      .append (slideElement);
  }

  /*
    Accepts one argument: itemElements, a jQuery
    HTML Element array that represents a set
    of item elements; and returns a jQuery
    HTML Element that represents a slide element
    containing itemElements.
  */
  function createSlideElement (itemElements) {
    return $('<div></div>')
      .addClass (getSlideElementClassName ())
      .append (itemElements)
      .draggable ({
        axis: 'x',
        containment: 'parent'
      });
  }

  /*
    Accepts two arguments:

    * feature, a Feature object
    * and targetElement, a jQuery HTML
      Element that represents a target element

    and returns a jQuery HTML Element array
    representing a set of item elements
    that, in turn, represent the target items
    in targetElement.
  */
  function createItemElements (feature, targetElement) {
    return getTargetItemElements (targetElement).map (
      function (index, targetItemElement) {
        return createItemElement (feature, index, $(targetItemElement));
    }).toArray ();
  }

  /*
    Accepts one argument: itemElement, a jQuery
    HTML Element that represents a slide item
    element; and returns true iff itemElement
    is active.
  */
  function itemElementIsActive (itemElement) {
    return itemElement.hasClass (getItemElementActiveClassName ());
  }

  /*
    Accepts one argument: itemElement, a jQuery
    HTML Element that represents a slide item
    element; and returns itemElement's index.
  */
  function getItemElementIndex (itemElement) {
    return itemElement.attr (getItemElementIndexAttribute ());
  }

  /*
    Accepts three arguments:

    * feature, a Feature object
    * index, a natural number
    * and targetItemElement, a jQuery HTML
      Element that represents a target item
      element

    and returns a jQuery HTML Element that
    represents a item element belonging
    to feature under index and representing
    targetItemElement.
  */
  function createItemElement (feature, index, targetItemElement) {
    return $('<div></div>')
      .addClass (getItemElementClassName ())
      .addClass (targetItemElementIsActive (targetItemElement) ?
        getItemElementActiveClassName () : null)
      .attr (getItemElementIndexAttribute (), index)
      .append (targetItemElement.clone (true))
      .focusin (function () { feature.slideTo (index); });
  }

  /*
    Accepts no arguments and returns a jQuery
    HTML Element that represents a focus element.
  */
  function createFocusElement () {
    return $('<div></div>').addClass (getFocusElementClassName ());
  }

  /*
    Accepts one argument: targetItemElement,
    a jQuery HTML Element that represents a
    target item element; and returns true
    iff targetItemElement is active.
  */
  function targetItemElementIsActive (targetItemElement) {
    return targetItemElement.hasClass (getTargetItemElementActiveClassName ());
  } 

  /*
    Accepts one argument: targetElement, a
    jQuery HTML Element that represents a target
    element; and returns a jQuery HTML Result Set
    that represents the target items in
    targetElement.
  */
  function getTargetItemElements (targetElement) {
    return $('.' + getTargetItemElementClassName (), targetElement);
  }

  /*
    Accepts no arguments and returns a jQuery
    HTML Result Set that represents the target
    elements marked for replacement.
  */
  function getTargetElements () {
    return $('.' + getTargetElementClassName ());
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label slide elements.
  */
  function getSlideElementClassName () {
    return getFeatureElementClassName () + '-slide';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label slide container
    elements.
  */
  function getSlideContainerElementClassName () {
    return getFeatureElementClassName () + '-slide-container';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label left slide button
    elements.
  */
  function getLeftSlideButtonElementClassName () {
    return getFeatureElementClassName () + '-left-slide-button';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label right slide
    button elements.
  */
  function getRightSlideButtonElementClassName () {
    return getFeatureElementClassName () + '-right-slide-button';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label slide button
    elements.
  */
  function getSlideButtonElementClassName () {
    return getFeatureElementClassName () + '-slide-button';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label focus elements.
  */
  function getFocusElementClassName () {
    return getFeatureElementClassName () + '-focus';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label the currently
    active item element.
  */
  function getItemElementActiveClassName () {
    return getItemElementClassName () + '-active';
  }

  /*
    Accepts no arguments and returns the name
    of the data attribute used to specify a
    item element's index.
  */
  function getItemElementIndexAttribute () {
    return getItemElementAttributePrefix () + '-index';
  }

  /*
    Accepts no arguments and returns the data
    attribute prefix used by item element
    attributes.
  */
  function getItemElementAttributePrefix () {
    return getFeatureElementAttributePrefix () + '-item';
  }

  /*
    Accepts no arguments and returns the name of
    the class used to label item elements.
  */
  function getItemElementClassName () {
    return getFeatureElementClassName () + '-item';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label feature elements.
  */
  function getFeatureElementClassName () {
    return getModuleClassPrefix () + '-feature';
  }

  /*
    Accepts no arguments and returns the
    attribute prefix used by feature element
    attributes.
  */
  function getFeatureElementAttributePrefix () {
    return getModuleAttributePrefix () + '-feature';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label active target items.
  */
  function getTargetItemElementActiveClassName () {
    return getTargetItemElementClassName () + '-active';
  }

  /*
    Accepts no arguments and returns the name of
    the class used to label target item elements.
  */
  function getTargetItemElementClassName () {
    return getTargetElementClassName () + '-item';
  }

  /*
    Accepts no arguments and returns the name
    of the class used to label target elements.
  */
  function getTargetElementClassName () {
    return getModuleClassPrefix () + '-target';
  }

  /*
    Accepts no arguments and returns the class
    prefix used by all classes defined by
    this module.
  */
  function getModuleClassPrefix () {
    return 'slide-format';
  }

  /*
    Accepts no arguments and returns the
    attribute prefix used by all of the data
    attributes created by this module.
  */
  function getModuleAttributePrefix () {
    return 'data-slide-format';
  }

}) (jQuery);
