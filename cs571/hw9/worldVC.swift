//
//  worldVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/4/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import SwiftSpinner
import SafariServices
import XLPagerTabStrip


class worldVC: UIViewController, UITableViewDataSource, UITableViewDelegate {
    
    
    @IBOutlet weak var newsTableView: UITableView!
    
    
    
    func delay(seconds: Double, completion: @escaping () -> ()) {
        let popTime = DispatchTime.now() + Double(Int64( Double(NSEC_PER_SEC) * seconds )) / Double(NSEC_PER_SEC)
        DispatchQueue.main.asyncAfter(deadline: popTime) {
            completion()
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        delay(seconds: 0.1) {
            SwiftSpinner.show(duration: 3.0, title: "Loading WORLD Headlines..")
        }
        self.configureTableView()
        fetchSectionNews(section: "world")
    }
    
    func configureTableView() {
        self.newsTableView.refreshControl = refresher
        self.newsTableView.dataSource = self
        self.newsTableView.delegate = self
        self.newsTableView.rowHeight = 143
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return newslist.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "worldNewsCell", for: indexPath) as! NewsTableViewCell
        cell.titleLabel.text = newslist[indexPath.row]["title"].stringValue
        cell.sectionLabel.text = "| " + newslist[indexPath.row]["section"].string!
        cell.timeLabel.text = getTimeDifference(publishTime: newslist[indexPath.row]["time"].string!)
        cell.news = newslist[indexPath.row]
        
        cell.backgroundColor = UIColor.lightGray
        cell.layer.borderColor = UIColor.gray.cgColor
        cell.layer.borderWidth = 1
        cell.layer.cornerRadius = 8
        cell.clipsToBounds = true

        if let url = newslist[indexPath.row]["image"].string {
            fetchImage(url: url, completionHandler: { image, _ in
                cell.newsImage.image = image
            })
        }

        return cell
    }
    
    @IBAction func bookmarkBtn(_ sender: bookmarkButton) {
        
        let toastMessage = sender.isSelected ? "Article bookmarked. Check out the Bookmarks tab to view" : "Article removed from the Bookmarks"
        self.view.makeToast(toastMessage, duration: 2.0, position: .bottom)
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        
        let detailedPageVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "tableDetail")as! TableDetailVC
        detailedPageVC.news = newslist[indexPath.row]

        self.navigationController?.pushViewController(detailedPageVC, animated: true)
    }
    
    func tableView(_ tableView: UITableView, contextMenuConfigurationForRowAt indexPath: IndexPath, point: CGPoint) -> UIContextMenuConfiguration? {
        return UIContextMenuConfiguration(identifier: nil, previewProvider: nil, actionProvider: { suggestedActions in

            return self.makeContextMenu(for: self.newslist[indexPath.row])
        })
    }
    
    
    lazy var refresher: UIRefreshControl = {
        let refreshControl = UIRefreshControl()
        refreshControl.addTarget(self, action: #selector(fetchData), for: .valueChanged)
        return refreshControl
    }()
    
    @objc func fetchData(){
        fetchSectionNews(section: "world")
        let ddl = DispatchTime.now() + .milliseconds(700)
        DispatchQueue.main.asyncAfter(deadline: ddl, execute: {
            self.refresher.endRefreshing()
        })
    }
    
    
    private var newslist = [JSON]()
    let cellSpacingHeight: CGFloat = 5
    
    func fetchSectionNews(section: String) {
        let urlToSearch = "https://cs571-hw9-2270250947.wm.r.appspot.com/api/guardian/section/\(section)"
        
        Alamofire.request(urlToSearch).responseJSON { (response) -> Void in
            if((response.result.value) != nil) {
                let json = JSON(response.result.value!)
                self.newslist = json["results"].arrayValue
            }
            DispatchQueue.main.async {
                self.newsTableView.reloadData()
            }
        }
    }
    
    
    func fetchImage(url: String, completionHandler: @escaping (UIImage?, NetworkError) -> ()) {
        Alamofire.request(url).responseData { responseData in
            
            guard let imageData = responseData.data else {
                completionHandler(nil, .failure)
                return
            }
            
            guard let image = UIImage(data: imageData) else {
                completionHandler(nil, .failure)
                return
            }
            
            completionHandler(image, .success)
        }
    }
    
    
    func getTimeDifference(publishTime: String) -> String {
        let currentDate = Date()
        let dateFormatter = DateFormatter()
        let userCalendar = Calendar.current
        let requestedComponent: Set<Calendar.Component> = [.year, .month, .day, .hour, .minute, .second]
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        guard let publishDate = dateFormatter.date(from: publishTime) else { return "" }
        let timeDifference = userCalendar.dateComponents(requestedComponent, from: publishDate, to: currentDate)
        let year = timeDifference.year!
        let month = timeDifference.month!
        let day = timeDifference.day! + 30 * (month + 12 * year)
        let hour = timeDifference.hour!
        let minute = timeDifference.minute!
        let second = timeDifference.second!
        if(day > 0){
            return "\(day)d ago"
        }
        else if(hour > 0){
            return "\(hour)h ago"
        }
        else if(minute > 0){
            return "\(minute)m ago"
        }
        return "\(second)s ago"
    }
    
    func makeContextMenu(for news: JSON) -> UIMenu {
        
        let twitter = UIAction(title: "Share With Twitter", image: UIImage(named: "twitter")) { action in
            let tweetText = "Check out this Article!"
            let tweetUrl = news["url"].stringValue
            let tweetHashtag = "CSCI_571_NewsApp"
            let shareString = "https://twitter.com/intent/tweet?text=\(tweetText)&url=\(tweetUrl)&hashtags=\(tweetHashtag)"

            let escapedShareString = shareString.addingPercentEncoding(withAllowedCharacters: CharacterSet.urlQueryAllowed)!
            let url = URL(string: escapedShareString)
            UIApplication.shared.open(url!)
        }
        
        let defaults = UserDefaults.standard
        let flag = defaults.object(forKey: news["id"].stringValue) as? Bool ?? false
        let bookmarkTitle = flag ? "Remove Bookmark" : "Bookmark"
        let bookmarkImage = flag ? UIImage(systemName: "bookmark.fill") : UIImage(systemName: "bookmark")
        let bookmark = UIAction(title: bookmarkTitle, image: bookmarkImage) { action in
            let toastMessage = flag ? "Article removed from the Bookmarks" : "Article bookmarked. Check out the Bookmarks tab to view"
            self.view.makeToast(toastMessage, duration: 2.0, position: .bottom)
            defaults.set(!flag, forKey: news["id"].stringValue)
            
            var bookmarks = defaults.object(forKey: "bookmarks") as? [String] ?? [String]()
            let index = self.find(id: news["id"].stringValue, array: bookmarks)
            if(flag){        // remove
                if(index == -1){
                    print("trying to delete an element which do not exist")
                }
                else{
                    bookmarks.remove(at: index)
                }
            }
            else{           // add into bookmarks
                if(index == -1){
                    bookmarks.append(news.rawString()!)
                }
                else{
                    print("element already in the array")
                }
            }
            defaults.set(bookmarks, forKey: "bookmarks")
        }
        
        return UIMenu(title: "Menu", children: [twitter, bookmark])
    }
    
    func find(id: String, array: [String]) -> Int {
        for (index, rawstring) in array.enumerated()
        {
            let json = JSON.init(parseJSON: rawstring)
            if json["id"].stringValue == id {
                return index
            }
        }

        return -1
    }
}


extension worldVC: IndicatorInfoProvider {
    func indicatorInfo(for pagerTabStripController: PagerTabStripViewController) -> IndicatorInfo {
        return IndicatorInfo(title: "WORLD")
    }
}

