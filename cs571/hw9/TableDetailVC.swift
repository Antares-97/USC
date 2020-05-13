//
//  TableDetailVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/4/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import Toast_Swift
import SwiftSpinner
import SafariServices

class TableDetailVC: UIViewController {

    
    @IBOutlet weak var newsImage: UIImageView!
    
    @IBOutlet weak var titleLabel: UILabel!
    
    @IBOutlet weak var sectionLabel: UILabel!
    
    @IBOutlet weak var dateLabel: UILabel!
    
    @IBOutlet weak var descriptionLabel: UILabel!
    
    
    var news = JSON()
    
    private var newsJSON:JSON = JSON()
    let defaults = UserDefaults.standard
    
    func delay(seconds: Double, completion: @escaping () -> ()) {
        let popTime = DispatchTime.now() + Double(Int64( Double(NSEC_PER_SEC) * seconds )) / Double(NSEC_PER_SEC)
        DispatchQueue.main.asyncAfter(deadline: popTime) {
            completion()
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        delay(seconds: 0.1) {
            SwiftSpinner.show(duration: 3.0, title: "Loading Detailed Article..")
        }
        configureNavigationBar()
        fetchNewsData()
    }
    
    func configureNavigationBar() {
        navigationItem.largeTitleDisplayMode = .never
        navigationItem.title = news["title"].stringValue
        
        let flag = defaults.object(forKey: news["id"].stringValue) as? Bool ?? false
        let bookmarkImage = flag ? UIImage(systemName: "bookmark.fill") : UIImage(systemName: "bookmark")
        let twitter = UIBarButtonItem(image: UIImage(named: "twitter"), style: .plain, target: self, action: #selector(twitterShare))
        let bookmark = UIBarButtonItem(image: bookmarkImage, style: .plain, target: self, action: #selector(bookMark))
        navigationItem.rightBarButtonItems = [twitter, bookmark]
    }
    
    @objc func twitterShare() {
        let tweetText = "Check out this Article!"
        let tweetUrl = news["url"].stringValue
        let tweetHashtag = "CSCI_571_NewsApp"
        let shareString = "https://twitter.com/intent/tweet?text=\(tweetText)&url=\(tweetUrl)&hashtags=\(tweetHashtag)"

        let escapedShareString = shareString.addingPercentEncoding(withAllowedCharacters: CharacterSet.urlQueryAllowed)!
        let url = URL(string: escapedShareString)
        UIApplication.shared.open(url!)
    }
    
    @objc func bookMark() {
        let flag = defaults.object(forKey: news["id"].stringValue) as? Bool ?? false
        let toastMessage = flag ? "Article removed from the Bookmarks" : "Article bookmarked. Check out the Bookmarks tab to view"
        self.view.makeToast(toastMessage, duration: 2.0, position: .bottom)
        
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
        defaults.set(!flag, forKey: news["id"].stringValue)
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
    
    
    func fetchNewsData(){
        
        let urlToSearch = "https://cs571-hw9-2270250947.wm.r.appspot.com/api/guardian/news/?id=\(news["id"].stringValue)"
        
        Alamofire.request(urlToSearch).responseJSON { (response) -> Void in
            if((response.result.value) != nil) {
                let json = JSON(response.result.value!)
                self.newsJSON = json["data"].self
            }
            DispatchQueue.main.async {
                self.titleLabel.text = self.newsJSON["title"].stringValue
                self.sectionLabel.text = self.newsJSON["section"].stringValue
                self.dateLabel.text = self.getDateFormat(originalDate: self.newsJSON["time"].stringValue)
                self.descriptionLabel.attributedText = self.getHTMLcontent(htmlText: self.newsJSON["description"].stringValue)
                
                if let url = self.newsJSON["image"].string {
                    self.fetchImage(url: url, completionHandler: { image, _ in
                        self.newsImage.image = image
                    })
                }
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
    
    
    func getDateFormat(originalDate:String) -> String {
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        guard let publishDate = dateFormatter.date(from: originalDate) else { return "" }
        dateFormatter.dateFormat = "dd MMM yyyy"
        let formattedDate = dateFormatter.string(from: publishDate)
        return formattedDate
    }
    
    
    func getHTMLcontent(htmlText: String) -> NSAttributedString {
        let htmlData = Data(htmlText.utf8)
        if let attributedString = try? NSAttributedString(data: htmlData, options: [.documentType: NSAttributedString.DocumentType.html], documentAttributes: nil) {
            return attributedString
        }
        return NSAttributedString()
    }
    
    
    @IBAction func viewFullArticle(_ sender: Any) {
        if let url = URL(string: self.newsJSON["url"].stringValue) {
            let articlePage = SFSafariViewController(url: url)
            present(articlePage, animated: true)
        }
    }
    

}
