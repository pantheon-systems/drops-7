/*

 * jQuery Menu plugin

 * Version: 0.0.9

 *

 * Copyright (c) 2007 Roman Weich

 * http://p.sohei.org

 *

 * Dual licensed under the MIT and GPL licenses 

 * (This means that you can choose the license that best suits your project, and use it accordingly):

 *   http://www.opensource.org/licenses/mit-license.php

 *   http://www.gnu.org/licenses/gpl.html

 *

 * Changelog: 

 * v 0.0.9 - 2008-01-19

 */



(function($)

{

	var menus = [], //list of all menus

		visibleMenus = [], //list of all visible menus

		activeMenu = activeItem = null,

		menuDIVElement = $('<div class="menu-div outerbox" style="position:absolute;top:0;left:0;display:none;"><div class="shadowbox1"></div><div class="shadowbox2"></div><div class="shadowbox3"></div></div>')[0],

		menuULElement = $('<ul class="menu-ul innerbox"></ul>')[0],

		menuItemElement = $('<li style="position:relative;"><div class="menu-item"></div></li>')[0],

		arrowElement = $('<img class="menu-item-arrow" />')[0],

		$rootDiv = $('<div id="root-menu-div" style="position:absolute;top:0;left:0;"></div>'), //create main menu div

		defaults = {

			// $.Menu options

			showDelay : 200,

			hideDelay : 200,

			hoverOpenDelay: 0,

			offsetTop : 0,

			offsetLeft : 0,

			minWidth: 0,

			onOpen: null,

			onClose: null,



			// $.MenuItem options

			onClick: null,

			arrowSrc: null,

			addExpando: false,

			

			// $.fn.menuFromElement options

			copyClassAttr: false

		};

	

	$(function(){

		$rootDiv.appendTo('body');

	});

	

	$.extend({

		MenuCollection : function(items) {

		

			this.menus = [];

		

			this.init(items);

		}

	});

	$.extend($.MenuCollection, {

		prototype : {

			init : function(items)

			{

				if ( items && items.length )

				{

					for ( var i = 0; i < items.length; i++ )

					{

						this.addMenu(items[i]);

						items[i].menuCollection = this;

					}

				}

			},

			addMenu : function(menu)

			{

				if ( menu instanceof $.Menu )

					this.menus.push(menu);

				

				menu.menuCollection = this;

			

				var self = this;

				$(menu.target).hover(function(){

					if ( menu.visible )

						return;



					//when there is an open menu in this collection, hide it and show the new one

					for ( var i = 0; i < self.menus.length; i++ )

					{

						if ( self.menus[i].visible )

						{

							self.menus[i].hide();

							menu.show();

							return;

						}

					}

				}, function(){});

			}

		}

	});



	

	$.extend({

		Menu : function(target, items, options) {

			this.menuItems = []; //all direct child $.MenuItem objects

			this.subMenus = []; //all subMenus from this.menuItems

			this.visible = false;

			this.active = false; //this menu has hover or one of its submenus is open

			this.parentMenuItem = null;

			this.settings = $.extend({}, defaults, options);

			this.target = target;

			this.$eDIV = null;

			this.$eUL = null;

			this.timer = null;

			this.menuCollection = null;

			this.openTimer = null;



			this.init();

			if ( items && items.constructor == Array )

				this.addItems(items);

		}

	});



	$.extend($.Menu, {

		checkMouse : function(e)

		{

			var t = e.target;



			//the user clicked on the target of the currenty open menu

			if ( visibleMenus.length && t == visibleMenus[0].target )

				return;

			

			//get the last node before the #root-menu-div

			while ( t.parentNode && t.parentNode != $rootDiv[0] )

				t = t.parentNode;



			//is the found node one of the visible menu elements?

			if ( !$(visibleMenus).filter(function(){ return this.$eDIV[0] == t }).length )

			{

				$.Menu.closeAll();

			}

		},

		checkKey : function(e)

		{

			switch ( e.keyCode )

			{

				case 13: //return

					if ( activeItem )

						activeItem.click(e, activeItem.$eLI[0]);

					break;

				case 27: //ESC

					$.Menu.closeAll();

					break;

				case 37: //left

					if ( !activeMenu )

						activeMenu = visibleMenus[0];

					var a = activeMenu;

					if ( a && a.parentMenuItem ) //select the parent menu and close the submenu

					{

						//unbind the events temporary, as we dont want the hoverout event to fire

						var pmi = a.parentMenuItem;

						pmi.$eLI.unbind('mouseout').unbind('mouseover');

						a.hide();

						pmi.hoverIn(true);

						setTimeout(function(){ //bind again..but delay it

							pmi.bindHover();

						});

					}

					else if ( a && a.menuCollection ) //select the previous menu in the collection

					{

						var pos,

							mcm = a.menuCollection.menus;

						if ( (pos = $.inArray(a, mcm)) > -1 )

						{

							if ( --pos < 0 )

								pos = mcm.length - 1;

							$.Menu.closeAll();

							mcm[pos].show();

							mcm[pos].setActive();

							if ( mcm[pos].menuItems.length ) //select the first item

								mcm[pos].menuItems[0].hoverIn(true);

						}

					}

					break;

				case 38: //up

					if ( activeMenu )

						activeMenu.selectNextItem(-1);

					break;

				case 39: //right

					if ( !activeMenu )

						activeMenu = visibleMenus[0];

					var m,

						a = activeMenu,

						asm = activeItem ? activeItem.subMenu : null;

					if ( a )

					{

						if ( asm && asm.menuItems.length ) //select the submenu

						{

							asm.show();

							asm.menuItems[0].hoverIn();

						}

						else if ( (a = a.inMenuCollection()) ) //select the next menu in the collection

						{

							var pos,

								mcm = a.menuCollection.menus;

							if ( (pos = $.inArray(a, mcm)) > -1 )

							{

								if ( ++pos >= mcm.length )

									pos = 0;

								$.Menu.closeAll();

								mcm[pos].show();

								mcm[pos].setActive();

								if ( mcm[pos].menuItems.length ) //select the first item

									mcm[pos].menuItems[0].hoverIn(true);

							}

						}

					}

					break;

				case 40: //down

					if ( !activeMenu )

					{

						if ( visibleMenus.length && visibleMenus[0].menuItems.length )

							visibleMenus[0].menuItems[0].hoverIn();

					}

					else

						activeMenu.selectNextItem();

					break;

			}

			if ( e.keyCode > 36 && e.keyCode < 41 )

				return false; //this will prevent scrolling

		},

		closeAll : function()

		{

			while ( visibleMenus.length )

				visibleMenus[0].hide();

		},

		setDefaults : function(d)

		{

			$.extend(defaults, d);

		},

		prototype : {

			/**

			 * create / initialize new menu

			 */

			init : function()

			{

				var self = this;

				if ( !this.target )

					return;

				else if ( this.target instanceof $.MenuItem )

				{

					this.parentMenuItem = this.target;

					this.target.addSubMenu(this);

					this.target = this.target.$eLI;

				}



				menus.push(this);



				//use the dom methods instead the ones from jquery (faster)

				this.$eDIV = $(menuDIVElement.cloneNode(1));

				this.$eUL = $(menuULElement.cloneNode(1));

				this.$eDIV[0].appendChild(this.$eUL[0]);

				$rootDiv[0].appendChild(this.$eDIV[0]);



				//bind events

				if ( !this.parentMenuItem )

				{

					$(this.target).click(function(e){

						self.onClick(e);

					}).hover(function(e){

						self.setActive();



						if ( self.settings.hoverOpenDelay )

						{

							self.openTimer = setTimeout(function(){

								if ( !self.visible )

									self.onClick(e);

							}, self.settings.hoverOpenDelay);

						}

					}, function(){

						if ( !self.visible )

							$(this).removeClass('activetarget');



						if ( self.openTimer )

							clearTimeout(self.openTimer);

					});

				}

				else

				{

					this.$eDIV.hover(function(){

						self.setActive();

					}, function(){});

				}

			},

			setActive : function()

			{

				if ( !this.parentMenuItem )

					$(this.target).addClass('activetarget');

				else

					this.active = true;

			},

			addItem : function(item)

			{

				if ( item instanceof $.MenuItem )

				{

					if ( $.inArray(item, this.menuItems) == -1 )

					{

						this.$eUL.append(item.$eLI);

						this.menuItems.push(item);

						item.parentMenu = this;

						if ( item.subMenu )

							this.subMenus.push(item.subMenu);

					}

				}

				else

				{

					this.addItem(new $.MenuItem(item, this.settings));

				}

			},

			addItems : function(items)

			{

				for ( var i = 0; i < items.length; i++ )

				{

					this.addItem(items[i]);

				}

			},

			removeItem : function(item)

			{

				var pos = $.inArray(item, this.menuItems);

				if ( pos > -1 )

					this.menuItems.splice(pos, 1);

				item.parentMenu = null;

			},

			hide : function()

			{

				if ( !this.visible )

					return;

				

				var i, 

					pos = $.inArray(this, visibleMenus);



				this.$eDIV.hide();



				if ( pos >= 0 )

					visibleMenus.splice(pos, 1);

				this.visible = this.active = false;



				$(this.target).removeClass('activetarget');



				//hide all submenus

				for ( i = 0; i < this.subMenus.length; i++ )

				{

					this.subMenus[i].hide();

				}



				//set all items inactive (e.g. remove hover class..)

				for ( i = 0; i < this.menuItems.length; i++ )

				{

					if ( this.menuItems[i].active )

						this.menuItems[i].setInactive();

				}



				if ( !visibleMenus.length ) //unbind events when the last menu was closed

					$(document).unbind('mousedown', $.Menu.checkMouse).unbind('keydown', $.Menu.checkKey);



				if ( activeMenu == this )

					activeMenu = null;

					

				if ( this.settings.onClose )

					this.settings.onClose.call(this);

			},

			show : function(e)

			{

				if ( this.visible )

					return;



				var zi, 

					pmi = this.parentMenuItem;



				if ( this.menuItems.length ) //show only when it has items

				{

					if ( pmi ) //set z-index

					{

						zi = parseInt(pmi.parentMenu.$eDIV.css('z-index'));

						this.$eDIV.css('z-index', (isNaN(zi) ? 1 : zi + 1));

					}

					this.$eDIV.css({visibility: 'hidden', display:'block'});



					//set min-width

					if ( this.settings.minWidth )

					{

						if ( this.$eDIV.width() < this.settings.minWidth )

							this.$eDIV.css('width', this.settings.minWidth);

					}

					

					this.setPosition();

					this.$eDIV.css({display:'none', visibility: ''}).show();



					//IEs default width: auto is bad! ie6 and ie7 have are producing different errors.. (7 = 5px shadowbox + 2px border)

					if ( $.browser.msie )

						this.$eUL.css('width', parseInt($.browser.version) == 6 ? this.$eDIV.width() - 7 : this.$eUL.width());



					if ( this.settings.onOpen )

						this.settings.onOpen.call(this);

				}

				if ( visibleMenus.length == 0 )

					$(document).bind('mousedown', $.Menu.checkMouse).bind('keydown', $.Menu.checkKey);



				this.visible = true;

				visibleMenus.push(this);

			},

			setPosition : function()

			{

				var $t, o, posX, posY, 

					pmo, //parent menu offset

					wst, //window scroll top

					wsl, //window scroll left

					ww = $(window).width(), 

					wh = $(window).height(),

					pmi = this.parentMenuItem,

					height = this.$eDIV[0].clientHeight,

					width = this.$eDIV[0].clientWidth,

					pheight; //parent height



				if ( pmi )

				{

					//position on the right side of the parent menu item

					o = pmi.$eLI.offset();

					posX = o.left + pmi.$eLI.width();

					posY = o.top;

				}

				else

				{

					//position right below the target

					$t = $(this.target);

					o = $t.offset();

					posX = o.left + this.settings.offsetLeft;

					posY = o.top + $t.height() + this.settings.offsetTop;

				}



				//y-pos

				if ( $.fn.scrollTop )

				{

					wst = $(window).scrollTop();

					if ( wh < height ) //menu is bigger than the window

					{

						//position the menu at the top of the visible area

						posY = wst;

					}

					else if ( wh + wst < posY + height ) //outside on the bottom?

					{

						if ( pmi )

						{

							pmo = pmi.parentMenu.$eDIV.offset();

							pheight = pmi.parentMenu.$eDIV[0].clientHeight;

							if ( height <= pheight )

							{

								//bottom position = parentmenu-bottom position

								posY = pmo.top + pheight - height;

							}

							else

							{

								//top position = parentmenu-top position

								posY = pmo.top;

							}

							//still outside on the bottom?

							if ( wh + wst < posY + height )

							{

								//shift the menu upwards till the bottom is visible

								posY -= posY + height - (wh + wst);

							}

						}

						else

						{

							//shift the menu upwards till the bottom is visible
