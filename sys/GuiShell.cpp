/* GuiShell.cpp
 *
 * Copyright (C) 1993-2018,2020-2022,2024,2025 Paul Boersma, 2013 Tom Naughton
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GuiP.h"
#include "../kar/UnicodeData.h"

Thing_implement (GuiShell, GuiForm, 0);

#if cocoa
	@implementation GuiCocoaShell {
		GuiShell d_userData;
	}
	- (void) dealloc {   // override
		if (Melder_debug == 55)
			Melder_casual (U"\t\tGuiCocoaShell-", Melder_pointer (self), U" dealloc");
		GuiShell me = d_userData;
		if (me) {
			my d_cocoaShell = nullptr;   // this is already under destruction, so undangle
			forget (me);
		}
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiShell));
		d_userData = static_cast <GuiShell> (userData);
	}
	- (void) keyDown: (NSEvent *) theEvent {
		trace (U"GuiCocoaShell: key down");
		[super keyDown: theEvent];   // for automatic behaviour in dialog boxes; do GuiWindows have to override this to do nothing?
	}
	- (BOOL) windowShouldClose: (id) sender {
		GuiCocoaShell *widget = (GuiCocoaShell *) sender;
		GuiShell me = (GuiShell) [widget getUserData];
		Melder_assert (me);
		if (my d_goAwayCallback) {
			trace (U"calling goAwayCallback)");
			my d_goAwayCallback (my d_goAwayBoss);
		} else {
			trace (U"hiding window or dialog");
			[widget orderOut: nil];
		}
		return false;
	}
	- (void) cancelOperation: (id) sender {
		trace (U"GuiCocoaShell: escape key pressed");
		GuiCocoaShell *widget = (GuiCocoaShell *) sender;
		GuiWindow me = (GuiWindow) [widget getUserData];
		if (me && my classInfo == classGuiWindow && my d_escapeCallback) {
			try {
				structGuiMenuItemEvent event { nullptr, false, false, false };
				my d_escapeCallback (my d_escapeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Cancelling in window not completely handled.");
			}
		} else {
			trace (U"calling the global escape callback (1)");
			if (theGuiEscapeMenuItemCallback) {
				try {
					trace (U"calling the global escape callback (2)");
					structGuiMenuItemEvent event { nullptr, false, false, false };
					theGuiEscapeMenuItemCallback (theGuiEscapeMenuItemBoss, & event);
				} catch (MelderError) {
					Melder_flushError (U"Cancelling not completely handled.");
				}
			}
		}
	}
	- (void) setDistinctiveBackGround {
		if (@available (macOS 10.15, *))
			[self   setBackgroundColor: [NSColor
				colorWithName: @"dynamicWindowBackgroundColour"
				dynamicProvider: ^ NSColor*_Nonnull (NSAppearance*_Nonnull appearance) {
					if ([appearance   bestMatchFromAppearancesWithNames: @[NSAppearanceNameAqua, NSAppearanceNameDarkAqua]] == NSAppearanceNameDarkAqua)
						return [NSColor   colorWithCalibratedWhite: 0.15   alpha: 1.0];   // dark mode, changing on the fly
					else
						return [NSColor   colorWithCalibratedWhite: 0.85   alpha: 1.0];   // light mode, changing on the fly
				}
			]];
		else
			[self   setBackgroundColor: [NSColor   colorWithCalibratedWhite: 0.85   alpha: 1.0]];   // not dynamic
	}
	@end
	@implementation GuiCocoaShellDelegate
	- (void) windowWillEnterFullScreen: (NSNotification *) notification {
		//TRACE
		trace (1);
	}
	- (void) windowDidEnterFullScreen: (NSNotification *) notification {
		//TRACE
		trace (1);
	}
	- (void) windowWillExitFullScreen: (NSNotification *) notification {
		//TRACE
		trace (1);
	}
	- (void) windowDidExitFullScreen: (NSNotification *) notification {
		//TRACE
		trace (1);
	}
	@end
#endif

void structGuiShell :: v9_destroy () noexcept {
	#if cocoa
		if (Melder_debug == 55)
			Melder_casual (U"\t", Thing_messageNameAndAddress (this), U" v9_destroy: cocoaShell ", Melder_pointer (our d_cocoaShell));
		if (our d_cocoaShell) {
			[our d_cocoaShell setUserData: nullptr];   // undangle reference to this
			[our d_cocoaShell orderOut: nil];
			[our d_cocoaShell close];
			[our d_cocoaShell release];
			our d_cocoaShell = nullptr;   // undangle
		}
	#endif
	GuiShell_Parent :: v9_destroy ();
}

int GuiShell_getShellWidth (GuiShell me) {
	int width = 0;
	#if gtk
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_gtkWindow), & allocation);   // TODO: replace with gtk_window_get_size()
		width = allocation.width;
	#elif motif
		width = my d_xmShell -> width;
	#elif cocoa
		width = [my d_cocoaShell   frame].size.width;
	#endif
	return width;
}

int GuiShell_getShellHeight (GuiShell me) {
	int height = 0;
	#if gtk
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_gtkWindow), & allocation);   // TODO: replace with gtk_window_get_size()
		height = allocation.height;
	#elif motif
		height = my d_xmShell -> height;
	#elif cocoa
		height = [my d_cocoaShell   frame].size.height;
	#endif
	return height;
}

void GuiShell_setTitle (GuiShell me, conststring32 title /* cattable */) {
	#if gtk
		gtk_window_set_title (my d_gtkWindow, Melder_peek32to8 (title));
		#if defined (chrome)
			if (my chrome_surrogateShellTitleLabelWidget) {
				char *markup = g_markup_printf_escaped ("<span weight=\"ultrabold\" underline=\"low\">%s</span>", Melder_peek32to8 (title));
				gtk_label_set_markup (GTK_LABEL (my chrome_surrogateShellTitleLabelWidget), markup);
				g_free (markup);
			}
		#endif
	#elif motif
		SetWindowTextW (my d_xmShell -> window, Melder_peek32toW (title));
	#elif cocoa
		[my d_cocoaShell   setTitle: (NSString *) Melder_peek32toCfstring (title)];
	#endif
}

void GuiShell_drain (GuiShell me, bool waitUntilItHasHappened, bool allowOtherEvents) {
	#if gtk
		//gdk_window_process_all_updates ();
		while (gtk_events_pending ())
			gtk_main_iteration ();
	#elif motif
		UpdateWindow (my d_xmShell -> window);
	#elif cocoa
		Melder_assert (my d_cocoaShell);
		Melder_assert ([NSThread isMainThread]);
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		[my d_cocoaShell   makeKeyAndOrderFront: nil];
		[my d_cocoaShell   layoutIfNeeded];
		[my d_cocoaShell   displayIfNeeded];
		//[CATransaction flush];   // in case we ever implement layers
		if (waitUntilItHasHappened) {
			constexpr double smallestExpectedScreenRefreshFrequency = 50.0;   // hertz
			constexpr double greatestExpectedScreenRefreshPeriod =
					1.0 / smallestExpectedScreenRefreshFrequency;   // e.g. 0.02 seconds
			constexpr double safeWaitingDurationForAtLeastOneScreenRefresh =
					1.5 * greatestExpectedScreenRefreshPeriod;   // e.g. 0.03 seconds
			const double timeByWhichAtLeastOneScreenRefreshHasOccurred =
					Melder_clock() + safeWaitingDurationForAtLeastOneScreenRefresh;
			while (Melder_clock() < timeByWhichAtLeastOneScreenRefreshHasOccurred) {
				NSEvent *nsEvent;
				while ((nsEvent = [NSApp
					nextEventMatchingMask: NSEventMaskAny
					untilDate: [NSDate distantPast]
					inMode: NSDefaultRunLoopMode
					dequeue: YES]) != nullptr)
				{
					if (allowOtherEvents) {
						[NSApp   sendEvent: nsEvent];
					} else {
						NSUInteger nsEventType = [nsEvent type];
						if (nsEventType == NSEventTypeKeyDown)
							NSBeep();
						/*
							Ignore all other events.
						*/
					}
				}
				constexpr double moderatePollingFrequency = 1000.0;   // hertz
				constexpr double moderatePollingPeriod = 1.0 / moderatePollingFrequency;   // e.g. 1 ms
				[NSThread   sleepForTimeInterval: moderatePollingPeriod];
			}
		} else {
			NSEvent *nsEvent;
			while ((nsEvent = [NSApp
				nextEventMatchingMask: NSEventMaskAny
				untilDate: [NSDate distantPast]
				inMode: NSDefaultRunLoopMode
				dequeue: YES]) != nullptr)
			{
				if (allowOtherEvents) {
					[NSApp   sendEvent: nsEvent];
				} else {
					NSUInteger nsEventType = [nsEvent type];
					if (nsEventType == NSEventTypeKeyDown)
						NSBeep();
					/*
						Ignore all other events.
					*/
				}
			}
		}
		[pool release];
	#endif
}

/* End of file GuiShell.cpp */
