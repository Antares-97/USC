//
//  HomeCollectionVC.swift
//  cs571_hw9
//
//  Created by LJZ's macbook pro on 5/2/20.
//  Copyright © 2020 LJZ@USC. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import SwiftSpinner
import CoreLocation

enum NetworkError: Error {
    case success
    case failure
}


class HomeCollectionVC: UICollectionViewController, CLLocationManagerDelegate, UITableViewDelegate {
    
    func delay(seconds: Double, completion: @escaping () -> ()) {
        let popTime = DispatchTime.now() + Double(Int64( Double(NSEC_PER_SEC) * seconds )) / Double(NSEC_PER_SEC)
        DispatchQueue.main.asyncAfter(deadline: popTime) {
            completion()
        }
    }
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        delay(seconds: 0.1) {
            SwiftSpinner.show(duration: 2.5, title: "Loading Home Page..")
        }
        title = "Home"
        configureSearchController()

        self.collectionView.delegate = self
        self.collectionView.dataSource = self
        self.collectionView.refreshControl = refresher
        
        locationManager.delegate = self
        locationManager.requestWhenInUseAuthorization()
        locationManager.startUpdatingLocation()
        locationManager.distanceFilter = 1000
        
        fetchWeather(city: self.city)
        fetchSectionNews(section: "home")
        
    }
    
    override func viewWillAppear(_ animated: Bool) {
        fetchData()
    }
    
    private let headerIdentifier = "collectionHeader"
    private let reuseIdentifier = "newsCollectionCell"
    private let itemsPerRow: CGFloat = 1
    private let sectionInsets = UIEdgeInsets(top: 5.0,
                                             left: 5.0,
                                             bottom: 5.0,
                                             right: 5.0)

    private var newslist = [JSON]()
    
    let cellSpacingHeight: CGFloat = 5
    
    let locationManager: CLLocationManager = CLLocationManager()
    private var currentLocation: CLLocation?
    var state:String = "CA"
    var city:String = "Los Angeles"
    var temperature: Int = 15
    var climate: String = "Default"
    
    lazy var refresher: UIRefreshControl = {
        let refreshControl = UIRefreshControl()
        refreshControl.addTarget(self, action: #selector(fetchData), for: .valueChanged)
        return refreshControl
    }()
    
    @objc func fetchData(){
        fetchWeather(city: self.city)
        fetchSectionNews(section: "home")
        let ddl = DispatchTime.now() + .milliseconds(700)
        DispatchQueue.main.asyncAfter(deadline: ddl, execute: {
            self.refresher.endRefreshing()
        })
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
        

    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        currentLocation = locations.last!
        lookUpCurrentLocation(for: currentLocation!, completionHandler: {
            placemark in
            guard let placemark = placemark else { return }
            if let state = placemark.administrativeArea {
                self.state = state
            }
            if let city = placemark.locality {
                self.city = city
            }
        })
    }
    
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        if let error = error as? CLError, error.code == .denied {
            manager.stopUpdatingLocation()
            print("Location Access Denied")
            return
        }
    }
    
    
    func lookUpCurrentLocation(for location: CLLocation, completionHandler: @escaping (CLPlacemark?) -> Void ) {
        let geocoder = CLGeocoder()
        geocoder.reverseGeocodeLocation(location) {
            placemarks, error in
            
            guard error == nil else {
                print("*** Error in \(#function): \(error!.localizedDescription)")
                completionHandler(nil)
                return
            }
            
            guard let placemark = placemarks?[0] else {
                print("*** Error in \(#function): placemark is nil")
                completionHandler(nil)
                return
            }
            
            completionHandler(placemark)
        }
    }
    
    
    func fetchWeather(city: String) {
        let query:String = city.replacingOccurrences(of: " ", with: "%20")
        let urlToSearch = "https://api.openweathermap.org/data/2.5/weather?q=\(query)&units=metric&appid=21c6e16a285ce789e5d5e157bfa852fc"
        
        Alamofire.request(urlToSearch).responseJSON { (response) -> Void in
            if((response.result.value) != nil) {
                let json = JSON(response.result.value!)
                self.climate = json["weather"].arrayValue[0]["main"].stringValue
                self.temperature = Int(round(json["main"]["temp"].doubleValue))
            }
        }

    }
    
    override func collectionView(_ collectionView: UICollectionView, viewForSupplementaryElementOfKind kind: String, at indexPath: IndexPath) -> UICollectionReusableView {
        switch kind {
            case UICollectionView.elementKindSectionHeader:
              guard
                let headerView = collectionView.dequeueReusableSupplementaryView(
                  ofKind: kind,
                  withReuseIdentifier: headerIdentifier,
                  for: indexPath) as? homeTabHeader
                else {
                  fatalError("Invalid view type")
              }
              headerView.cityLabel.text = self.city
              headerView.stateLabel.text = self.state
              headerView.climateLabel.text = self.climate
              headerView.temperatureLabel.text = String(self.temperature) + "°C"
              var backgroundWeatherImage: String
              switch self.climate {
                  case "Clouds":
                    backgroundWeatherImage = "cloudy_weather"
                  case "Clear":
                    backgroundWeatherImage = "clear_weather"
                  case "Snow":
                    backgroundWeatherImage = "snowy_weather"
                  case "Rain":
                    backgroundWeatherImage = "rainy_weather"
                  case "Thunderstorm":
                    backgroundWeatherImage = "thunder_weather"
                  default:
                    backgroundWeatherImage = "sunny_weather"
              }
              headerView.weatherImage.image = UIImage(named: backgroundWeatherImage)
              return headerView
            
            default:
              assert(false, "Invalid element type")
        }
    }


    override func numberOfSections(in collectionView: UICollectionView) -> Int {
        return 1
    }


    override func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return newslist.count
    }
    
    @IBAction func bookmarkBtn(_ sender: bookmarkButton) {
        
        let toastMessage = sender.isSelected ? "Article bookmarked. Check out the Bookmarks tab to view" : "Article removed from the Bookmarks"
        self.view.makeToast(toastMessage, duration: 2.0, position: .bottom)
    }
    

    override func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: reuseIdentifier, for: indexPath) as! NewsCollectionViewCell
    
        cell.titleLabel.text = newslist[indexPath.row]["title"].stringValue
        cell.sectionLabel.text = "| " + newslist[indexPath.row]["section"].stringValue
        cell.timeLabel.text = getTimeDifference(publishTime: newslist[indexPath.row]["time"].string!)
        cell.bookmark = bookmarkButton(newsID: newslist[indexPath.row]["id"].stringValue)
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
    
    override func collectionView(_ collectionView: UICollectionView, contextMenuConfigurationForItemAt indexPath: IndexPath, point: CGPoint) -> UIContextMenuConfiguration? {
        
        return UIContextMenuConfiguration(identifier: nil, previewProvider: nil, actionProvider: { suggestedActions in

            return self.makeContextMenu(for: self.newslist[indexPath.row])
        })
        
    }

    override func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let detailedPageVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "collectionDetail") as! CollectionDetailVC
        detailedPageVC.news = newslist[indexPath.row]

        self.navigationController?.pushViewController(detailedPageVC, animated: true)
    }

    
    func fetchSectionNews(section: String) {
        let urlToSearch = "https://cs571-hw9-2270250947.wm.r.appspot.com/api/guardian/section/\(section)"
        
        Alamofire.request(urlToSearch).responseJSON { (response) -> Void in
            if((response.result.value) != nil) {
                let json = JSON(response.result.value!)
                self.newslist = json["results"].arrayValue
            }
            DispatchQueue.main.async {
                self.collectionView.reloadData()
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


extension HomeCollectionVC: UICollectionViewDelegateFlowLayout {
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        
        let paddingSpace = sectionInsets.left * (itemsPerRow + 1)
        let availableWidth = view.frame.width - paddingSpace
        let widthPerItem = availableWidth / itemsPerRow
        
        return CGSize(width: widthPerItem, height: 143.0)
    }
    
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, insetForSectionAt section: Int) -> UIEdgeInsets {
        return sectionInsets
    }
    
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, minimumInteritemSpacingForSectionAt section: Int) -> CGFloat {
        return sectionInsets.left
    }
    
}

