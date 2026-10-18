//
//  SceneDelegate.swift
//  Farert
//
//  Scene life cycle delegate (required by the iOS 27 SDK).
//  Builds the root window (slide menu + main navigation) and manages DB open/close on foreground/background.
//

import UIKit

class SceneDelegate: UIResponder, UIWindowSceneDelegate {

    var window: UIWindow?

    /// True after the DB was closed on background; sceneWillEnterForeground is also called on first launch,
    /// so the DB (already opened in AppDelegate) must be reopened only when this flag is set.
    private var isDatabaseClosed = false

    /// Creates the main window and installs the slide menu as its root view controller.
    /// - Parameters:
    ///   - scene: The scene being connected to the app.
    ///   - session: The session containing the scene configuration.
    ///   - connectionOptions: Options passed when the scene was connected.
    func scene(_ scene: UIScene, willConnectTo session: UISceneSession, options connectionOptions: UIScene.ConnectionOptions) {
        guard let windowScene = scene as? UIWindowScene else {
            return
        }

        let storyboard = UIStoryboard(name: "Main", bundle: nil)

        let leftViewController = storyboard.instantiateViewController(withIdentifier: "LeftViewController") as! LeftTableViewController

        let nvc : UINavigationController = storyboard.instantiateViewController(withIdentifier: "MainNavi") as! UINavigationController

        leftViewController.mainViewController = nvc

        let slideMenuController = SlideMenuController(mainViewController: nvc, leftMenuViewController: leftViewController)

        let window = UIWindow(windowScene: windowScene)
        window.rootViewController = slideMenuController
        self.window = window
        // Existing view controllers refer to AppDelegate.window; keep it in sync (single-window app).
        (UIApplication.shared.delegate as? AppDelegate)?.window = window
        window.makeKeyAndVisible()
    }

    /// Reopens the database when the scene returns from the background.
    /// - Parameter scene: The scene entering the foreground.
    func sceneWillEnterForeground(_ scene: UIScene) {
        guard isDatabaseClosed else {
            return
        }
        if !cRouteUtil.openDatabase() {
            NSLog("Farert: failed to reopen database on entering foreground")
            return
        }
        isDatabaseClosed = false
    }

    /// Closes the database when the scene moves to the background.
    /// - Parameter scene: The scene entering the background.
    func sceneDidEnterBackground(_ scene: UIScene) {
        cRouteUtil.closeDatabase()
        isDatabaseClosed = true
    }
}
