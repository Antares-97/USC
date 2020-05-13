//
//  searchResultsTableVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/4/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class choicesTableVC: UIViewController {


    @IBOutlet weak var choicesTableView: UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.choicesTableView.dataSource = self
        self.choicesTableView.delegate = self
        self.choicesTableView.rowHeight = 53
    }

    
    var results:[String] = [String]()
    
    func filterDataSource(keyword: String) {
        if keyword.count > 2 {
            let headers: HTTPHeaders = [
                "Ocp-Apim-Subscription-Key": "d25921da6bf746a5a9d257eca0c390d8"
            ]
            Alamofire.request("https://api.cognitive.microsoft.com/bing/v7.0/suggestions?q=\(keyword)", method: .get, headers: headers).responseJSON { (responseData) -> Void in
                if((responseData.result.value) != nil) {
                    let json = JSON(responseData.result.value!)
                    var options: [String] = []
                    for (_, item): (String, JSON) in json["suggestionGroups"][0]["searchSuggestions"] {
                        options.append(item["displayText"].stringValue)
                    }
                    self.results = options
                }
                DispatchQueue.main.async {
                    self.choicesTableView.reloadData()
                }
            }
        }
    }
    
    
    func restoreCurrentDataSource() {
        results = [String]()
        choicesTableView.reloadData()
    }
}


extension choicesTableVC: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return results.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "choiceCell", for: indexPath) as! choiceTableViewCell

        cell.choiceLabel.text = results[indexPath.row].self
        return cell
    }
    
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        
        DispatchQueue.main.async {
            let searchResultPageVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "searchResultsTable")as! searchResultsVC
            searchResultPageVC.keyword = self.results[indexPath.row].self
            
            self.presentingViewController?.navigationController?.pushViewController(searchResultPageVC, animated: true)
        }
    }
    
}


extension choicesTableVC: UISearchResultsUpdating {
    
    func updateSearchResults(for searchController: UISearchController) {
        if let searchText = searchController.searchBar.text {
            filterDataSource(keyword: searchText)
        }
    }
    
}

