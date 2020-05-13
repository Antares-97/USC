//
//  HeadlinesVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/3/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import XLPagerTabStrip


class HeadlinesVC: ButtonBarPagerTabStripViewController {

    @IBOutlet weak var myButtonPagerTab: ButtonBarView!
    
    override func viewDidLoad() {
        self.configurePager()
        super.viewDidLoad()
        title = "Headlines"
        self.configureSearchController()
    }
    
    
    override func viewControllers(for pagerTabStripController: PagerTabStripViewController) -> [UIViewController] {
        let worldViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "world")
        let businessViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "business")
        let politicsViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "politics")
        let sportsViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "sports")
        let techViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "tech")
        let scienceViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "science")
        
        return [worldViewController, businessViewController, politicsViewController, sportsViewController, techViewController, scienceViewController]
    }

    
    func configurePager(){
        self.settings.style.selectedBarHeight = 3
        self.settings.style.selectedBarBackgroundColor = UIColor.link
        self.settings.style.buttonBarBackgroundColor = UIColor.white
        self.settings.style.buttonBarItemBackgroundColor = UIColor.white
        self.settings.style.buttonBarItemTitleColor = UIColor.gray
        self.settings.style.buttonBarItemFont = .boldSystemFont(ofSize: 15)
        self.settings.style.buttonBarMinimumLineSpacing = 0.0
        self.settings.style.buttonBarItemsShouldFillAvailableWidth = true
        self.settings.style.buttonBarLeftContentInset = 1.0
        self.settings.style.buttonBarRightContentInset = 1.0
        
        changeCurrentIndexProgressive = { (oldCell: ButtonBarViewCell?, newCell: ButtonBarViewCell?, progressPercentage: CGFloat, changeCurrentIndex: Bool, animated: Bool) -> Void in
            guard changeCurrentIndex == true else { return }
            oldCell?.label.textColor = UIColor.gray
            newCell?.label.textColor = UIColor.link
        }
    }
    
    
    var searchController: UISearchController!

    func configureSearchController() {
            
        let choicesTableController = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "choicesTable") as! choicesTableVC
        
        searchController = UISearchController(searchResultsController: choicesTableController)
        searchController.searchResultsUpdater = choicesTableController
        searchController.searchBar.autocapitalizationType = .none
        searchController.searchBar.placeholder = "Enter keyword.."
        searchController.obscuresBackgroundDuringPresentation = true
        searchController.definesPresentationContext = true
        searchController.hidesNavigationBarDuringPresentation = true
        
        navigationItem.searchController = searchController
        navigationItem.hidesSearchBarWhenScrolling = true
        navigationController?.definesPresentationContext = true
        
    }
}
