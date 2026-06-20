//
//  SettingsTableViewController.swift
//  iFarert Option Change database source management view
//
//  Created by TAKEDA, Noriyuki on 2015/03/27.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

private struct BackupDocument: Codable {
    let version: String
    let exportedAt: String
    let storage: BackupStorage
}

private struct BackupStorage: Codable {
    let currentRoute: String?
    let savedRoutes: [String]
    let ticketHolder: [BackupTicketHolder]
    let stationHistory: [String]
}

private struct BackupTicketHolder: Codable {
    let order: Int
    let routeScript: String
    let fareType: String
}

private struct RestoreSnapshot {
    let savedRoutes: [String]
    let ticketHolder: [(routeScript: String, fareType: String)]
    let stationHistory: [String]
}

class SettingsTableViewController: UITableViewController {

    // MARK: - Public property
//    var selectDbId : Int = 0

    // MARK: - Private property
//    var before_dbid_idx : Int = DB._MAX_ID.rawValue
    
    var isSameShinkanzanKokuraHakataOther : Bool = false;
    private var restoreJsonText: String?
    
    @IBOutlet weak var btnResetInfoMessage: UIButton!
    @IBOutlet weak var btnBackup: UIButton!
    @IBOutlet weak var btnRestore: UIButton!
    
    // MARK: - UI Propery
//    @IBOutlet weak var swShinkansenKokuraHakataOther: UISwitch!
    
//    @IBOutlet weak var sgmDataVer: UISegmentedControl!

    // MARK: Method
    
    override func viewDidLoad() {

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
        super.viewDidLoad()
        tableView.allowsSelection = true
        
        // Uncomment the following line to preserve selection between presentations.
        // self.clearsSelectionOnViewWillAppear = NO;

        self.navigationController?.isToolbarHidden = false

        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
/*
        before_dbid_idx = cRouteUtil.getDatabaseId().rawValue
        if ((before_dbid_idx < DB._MIN_ID.rawValue) || (DB._MAX_ID.rawValue < before_dbid_idx)) {
            before_dbid_idx = DB._MAX_ID.rawValue
        }
 
        self.selectDbId = -1;   /* is no select */
        self.sgmDataVer.selectedSegmentIndex = before_dbid_idx - DB._MIN_ID.rawValue;
 */
    
    //    self.swShinkansenKokuraHakataOther.setOn(isSameShinkanzanKokuraHakataOther, animated: false);
        
        var b : Bool = false
        let keys = [ "setting_key_hide_osakakan_detour_info", "setting_key_hide_no_rule_info", "import_guide"]
        for (_, k) in keys.enumerated() {
            if (cRouteUtil.read(fromKey: k) != nil) {
                b = true
                break
            }
        }
        if b {
            // どっちか1つでも抑制されてたら復活できるようにボタン有効
            btnResetInfoMessage.isEnabled = true
        } else {
            btnResetInfoMessage.isEnabled = false
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source
    /*
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 0
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // #warning Incomplete method implementation.
        // Return the number of rows in the section.
        return 0
    }
    */
    
    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        if (section == 0) {
            return "設定"
            //return "データソース"
        } else if (section == 1) {
            return "設定"
        } else if (section == 2) {
            return "その他"
        }
        return nil
    }

    /*
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("reuseIdentifier", forIndexPath: indexPath) as UITableViewCell

        // Configure the cell...

        return cell
    }
    */

    /*
    // Override to support conditional editing of the table view.
    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if editingStyle == .Delete {
            // Delete the row from the data source
            tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: .Fade)
        } else if editingStyle == .Insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the item to be re-orderable.
        return true
    }
    */


    // MARK: - Navigation

    @IBAction func actBtnResetInfoMessageTouched(_ sender: UIButton) {
        let keys = [ "setting_key_hide_osakakan_detour_info", "setting_key_hide_no_rule_info", "import_guide"]
        for (_, k) in keys.enumerated() {
            cRouteUtil.save(toKey: k, value: "", sync: true)
        }
        sender.isEnabled = false
    }
    
    @IBAction func actBtnBackupTouched(_ sender: UIButton) {
        do {
            let backup = try makeBackupJson()
            shareBackup(backup, from: sender)
        } catch {
            showAlert(title: "バックアップ失敗", message: error.localizedDescription)
        }
    }

    @IBAction func actBtnRestoreTouched(_ sender: UIButton) {
        guard let text = UIPasteboard.general.string else {
            showAlert(title: "リストアできません", message: "クリップボードにバックアップ JSON がありません")
            return
        }
        do {
            _ = try validateBackupDocument(text)
            restoreJsonText = text
        } catch {
            showAlert(title: "リストアできません", message: error.localizedDescription)
            return
        }
        let alert = UIAlertController(
            title: "リストア",
            message: "クリップボードのバックアップ JSON で現在の発着駅履歴、保存経路、きっぷホルダを置き換えます。",
            preferredStyle: .alert
        )
        alert.addAction(UIAlertAction(title: "キャンセル", style: .cancel) { [weak self] _ in
            self?.restoreJsonText = nil
        })
        alert.addAction(UIAlertAction(title: "リストア", style: .destructive) { [weak self] _ in
            self?.restoreFromPasteboard()
        })
        present(alert, animated: true)
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        if indexPath.section == 1 && indexPath.row == 0 {
            actBtnBackupTouched(btnBackup)
        } else if indexPath.section == 1 && indexPath.row == 1 {
            actBtnRestoreTouched(btnRestore)
        }
    }

    private func makeBackupJson() throws -> String {
        let holder = Routefolder()
        let ticketHolder = holder.backupItems().enumerated().map { index, item in
            BackupTicketHolder(
                order: index + 1,
                routeScript: item.routeScript,
                fareType: item.fareType
            )
        }
        let formatter = ISO8601DateFormatter()
        formatter.formatOptions = [.withInternetDateTime, .withFractionalSeconds]
        let document = BackupDocument(
            version: "1.0",
            exportedAt: formatter.string(from: Date()),
            storage: BackupStorage(
                currentRoute: nil,
                savedRoutes: cRouteUtil.loadStrageRoute() as? [String] ?? [],
                ticketHolder: ticketHolder,
                stationHistory: cRouteUtil.readFromTerminalHistory() as? [String] ?? []
            )
        )
        let encoder = JSONEncoder()
        encoder.outputFormatting = [.prettyPrinted, .sortedKeys]
        let data = try encoder.encode(document)
        guard let json = String(data: data, encoding: .utf8) else {
            throw NSError(domain: "FarertBackup", code: 1, userInfo: [NSLocalizedDescriptionKey: "JSON の作成に失敗しました"])
        }
        return json
    }

    private func shareBackup(_ backup: String, from sender: UIButton) {
        let activityController = UIActivityViewController(activityItems: [backup], applicationActivities: nil)
        activityController.setValue("Farert backup", forKey: "subject")
        activityController.popoverPresentationController?.sourceView = sender
        activityController.popoverPresentationController?.sourceRect = sender.bounds
        present(activityController, animated: true)
    }

    private func validateBackupDocument(_ text: String) throws -> BackupDocument {
        guard !text.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty,
              let data = text.data(using: .utf8) else {
            throw NSError(domain: "FarertBackup", code: 1, userInfo: [NSLocalizedDescriptionKey: "クリップボードにバックアップ JSON がありません"])
        }
        do {
            let document = try JSONDecoder().decode(BackupDocument.self, from: data)
            guard document.version == "1.0" else {
                throw NSError(domain: "FarertBackup", code: 2, userInfo: [NSLocalizedDescriptionKey: "未対応のバックアップバージョンです"])
            }
            return document
        } catch let error as NSError where error.domain == "FarertBackup" {
            throw error
        } catch {
            throw NSError(domain: "FarertBackup", code: 3, userInfo: [NSLocalizedDescriptionKey: "クリップボードの内容は Farert のバックアップ JSON として読み込めません"])
        }
    }

    private func restoreFromPasteboard() {
        guard let text = restoreJsonText ?? UIPasteboard.general.string else {
            showAlert(title: "リストア失敗", message: "クリップボードにバックアップ JSON がありません")
            return
        }
        do {
            let document = try validateBackupDocument(text)
            let savedRoutes = validSavedRoutes(document.storage.savedRoutes)
            let ticketHolder = validTicketHolder(document.storage.ticketHolder)
            let history = validStationHistory(document.storage.stationHistory)
            let snapshot = RestoreSnapshot(
                savedRoutes: cRouteUtil.loadStrageRoute() as? [String] ?? [],
                ticketHolder: Routefolder().backupItems(),
                stationHistory: cRouteUtil.readFromTerminalHistory() as? [String] ?? []
            )
            do {
                try restoreSavedRoutes(savedRoutes)
                try restoreTicketHolder(ticketHolder)
                try restoreStationHistory(history)
            } catch {
                rollbackRestore(snapshot)
                throw error
            }
            showAlert(
                title: "リストア",
                message: "リストアしました: 保存経路 \(savedRoutes.count)件、きっぷホルダ \(ticketHolder.count)件、履歴 \(history.count)件"
            )
            restoreJsonText = nil
        } catch {
            showAlert(title: "リストア失敗", message: error.localizedDescription)
        }
    }

    private func validSavedRoutes(_ routes: [String]) -> [String] {
        var restored: [String] = []
        for script in routes {
            let trimmed = script.trimmingCharacters(in: .whitespacesAndNewlines)
            if trimmed.isEmpty || restored.contains(trimmed) {
                continue
            }
            guard let route = cRoute(), route.setupRoute(trimmed) >= 0 else {
                continue
            }
            restored.append(route.routeScript() ?? trimmed)
            if Int(MAX_ARCHIVE_ROUTE) <= restored.count {
                break
            }
        }
        return restored
    }

    private func validTicketHolder(_ items: [BackupTicketHolder]) -> [(routeScript: String, fareType: String)] {
        let orderedItems = items.sorted { $0.order < $1.order }
        var restored: [(routeScript: String, fareType: String)] = []
        for item in orderedItems {
            guard restored.count < Int(MAX_HOLDER) else {
                break
            }
            let trimmed = item.routeScript.trimmingCharacters(in: .whitespacesAndNewlines)
            guard let route = cRoute(), route.setupRoute(trimmed) >= 0 else {
                continue
            }
            restored.append((routeScript: route.routeScript() ?? trimmed, fareType: item.fareType))
        }
        return restored
    }

    private func validStationHistory(_ history: [String]) -> [String] {
        var restored: [String] = []
        for station in history {
            let trimmed = station.trimmingCharacters(in: .whitespacesAndNewlines)
            if trimmed.isEmpty || restored.contains(trimmed) {
                continue
            }
            restored.append(trimmed)
            if FGD.MAX_HISTORY <= restored.count {
                break
            }
        }
        return restored
    }

    private func restoreSavedRoutes(_ routes: [String]) throws {
        cRouteUtil.save(toRouteArray: routes)
    }

    private func restoreTicketHolder(_ items: [(routeScript: String, fareType: String)]) throws {
        _ = Routefolder().restoreBackupItems(items)
    }

    private func restoreStationHistory(_ history: [String]) throws {
        cRouteUtil.saveToTerminalHistory(with: history)
    }

    private func rollbackRestore(_ snapshot: RestoreSnapshot) {
        cRouteUtil.save(toRouteArray: snapshot.savedRoutes)
        _ = Routefolder().restoreBackupItems(snapshot.ticketHolder)
        cRouteUtil.saveToTerminalHistory(with: snapshot.stationHistory)
    }

    private func showAlert(title: String, message: String) {
        let alert = UIAlertController(title: title, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default))
        present(alert, animated: true)
    }

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if (segue.identifier == "settingsSegue") {
/* defunct
            var dbid = self.sgmDataVer.selectedSegmentIndex + DB._MIN_ID.rawValue;
            if ((dbid < DB._MIN_ID.rawValue) || (DB._MAX_ID.rawValue < dbid)) {
                dbid = DB._MAX_ID.rawValue
            }
            if (before_dbid_idx != dbid) {
                cRouteUtil.save(toDatabaseId: dbid)
                cRouteUtil.closeDatabase()
                cRouteUtil.openDatabase(/*DB(rawValue: dbid)!*/)
//                self.selectDbId = dbid
            } else {
//                self.selectDbId = -1;   /* no change */
            }
 */
            /*
            let bKokuraHakataShinzai = self.swShinkansenKokuraHakataOther.isOn
            if (self.isSameShinkanzanKokuraHakataOther != bKokuraHakataShinzai) {
                self.isSameShinkanzanKokuraHakataOther = bKokuraHakataShinzai
                let tf = bKokuraHakataShinzai ? "true" : ""
                cRouteUtil.save(toKey: "kokura_hakata_shinzai", value: tf, sync: true)
            }
             */
        }
    }
    
}
