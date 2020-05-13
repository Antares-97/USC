//
//  searchResultsTableVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/4/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit

class choicesTableVC: UIViewController {

    @IBOutlet weak var choicesTable: UITableView!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        choicesTable.dataSource = self
        choicesTable.delegate = self
    }
    
    var results:[String] = [String]()
    
}


extension choicesTableVC: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return results.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "searchResultCell", for: indexPath) as! searchTableViewCell
        cell.choiceLabel.text = results[indexPath.row].self
        return cell
    }
    
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        print(results[indexPath.row].self)
    }
    
}
