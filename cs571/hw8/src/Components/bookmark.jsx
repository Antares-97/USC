import React, { Component } from 'react';
import { MdDelete } from "react-icons/md";
import TagBadge from './badge';
import ShareModal from './share';
import { withRouter } from 'react-router-dom';
import { toast } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';


toast.configure({
    autoClose: 2000,
    draggable: false,
    position: "top-center",
    hideProgressBar: true,
    bodyClassName: "article-toast",
});


class bookmarkpage extends Component {
    constructor(props){
        super(props);
        this.state = {
            results: [],
        };
    }

    componentWillMount(){
        this.props.handleShowSwitch(false);
        this.props.handleClick(true, 'bookmark');
        if(this.props.handleClear!==null)
            this.props.handleClear();
        let favorites = window.localStorage.getItem('favorites');
        favorites = JSON.parse(favorites);
        this.setState({
            results: favorites
        });
    }

    handleNewsClick = (id) => {
        let url = '/news?id=' + id;
        this.props.history.push(url);
    }

    handleDelete = (e, news) => {
        e.stopPropagation();
        toast('Removing ' + news.title);
        this.props.handleBookMark(true, news);
        let favorites = window.localStorage.getItem('favorites');
        favorites = JSON.parse(favorites);
        this.setState({
            results: favorites
        });
    }

    customSubstr(str, br){
        if(br+7 >= str.length)
            return str;
        if(br < str.length && str.charAt(br) !== ' ')
            br = str.indexOf(' ', br-3);
        return str.substr(0, br) + "...";
    }

    render() { 
        const newslist = this.state.results;
        if(newslist.length===0){
            return(
                <div style={{textAlign: "center", fontSize: "23px"}}>
                    You have no saved articles
                </div>
            );
        }

        return(
            <div className="page">
                <h5>Favorites</h5>
                {newslist.map((news) => (
                    <div className={"news"} key={news.id} onClick={() => this.handleNewsClick(news.id)}>
                        <div className="title">
                            {this.customSubstr(news.title, 43)}
                            <ShareModal 
                                title={<div><h2>{news.source}</h2> <p>{news.title}</p></div>} 
                                url={news.url}
                            />
                            <button onClick={event => this.handleDelete(event, news)}>
                                <MdDelete style={{fontSize: "19px"}}/>
                            </button>
                        </div>
                        

                        <div className="img">
                            <img src={news.image} alt="" />
                        </div>

                        <div className="text">
                            <h2>
                                {news.date}
                                <span className="tag"><TagBadge tag={news.section} style={{fontSize: "11px"}}/> <TagBadge tag={news.source} style={{fontSize: "11px"}}/></span>
                            </h2>
                        </div>
                    </div>
                ))}
            </div>
        );
    }
}
 
export default withRouter(bookmarkpage);