import React, { Component } from 'react';
import axios from 'axios';
import LoadingPage from './loader';
import {FacebookShareButton, 
        TwitterShareButton, 
        EmailShareButton,
        FacebookIcon,
        TwitterIcon,
        EmailIcon,
    } from 'react-share';
import { MdBookmark, MdBookmarkBorder, MdKeyboardArrowDown, MdKeyboardArrowUp } from 'react-icons/md';
import OverlayTrigger from 'react-bootstrap/OverlayTrigger'
import Tooltip from 'react-bootstrap/Tooltip';
import { toast } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import queryString from 'query-string';
import CommentBox from './commentbox';


toast.configure({
    autoClose: 2000,
    draggable: false,
    position: "top-center",
    hideProgressBar: true,
    bodyClassName: "article-toast",
});


class Article extends Component {
    constructor(props){
        super(props);
        this.state = {
            news: {},
            marked: false,
            id: '',
            expanded: false,
        };
        this.myRef = React.createRef();
        this.handleBookMark.bind(this);
        this.handleExpand.bind(this);
    }

    componentDidMount(){
        this.props.handleShowSwitch(false);
        this.props.handleClick(false, 'news');
        if(this.props.handleClear!==null)
            this.props.handleClear();
        const queryParams = queryString.parse(this.props.location.search);
        const id = queryParams.id;
        const flag = window.localStorage.getItem(id+'-marked')==="true"? true: false;
        window.localStorage.setItem(id+'-marked', flag);
        this.setState({
            id: queryParams.id,
            marked: flag,
        });
        const source = id.substring(0,4) === "http"? false: true;
        const url = 'https://cs571-hw8-2270250947.appspot.com/api/' + (source? 'guardian': 'nytimes') + '/news/?id=' + id;
        axios.get(url)
        .then(resp => {
            this.setState({news: resp.data.data}); 
        })
    }

    handleBookMark = (title) => {
        this.setState({
            marked: !this.state.marked
        });
        this.props.handleBookMark(this.state.marked, this.state.news);
        let message = (this.state.marked? "Removing ": "Saving ") + title;
        toast(message);
        window.localStorage.setItem(this.state.id+'-marked', (this.state.marked? "false": "true"));
    }

    handleExpand = (ref) => {
        ref.current.scrollIntoView({behavior: 'smooth'});
        this.setState({
            expanded: !this.state.expanded
        });
    }

    render() {
        const article = this.state.news;
        if(!article.hasOwnProperty('id')){
            return(
                <div className="page">
                    <LoadingPage />
                </div>
            );
        }

        return (
            <div className="page">
                <div className="article">
                    <div className="text">
                        <h1>{article.title}</h1>
                        <h2>
                            {article.date}
                            <span className="tag">
                                <OverlayTrigger overlay={<Tooltip id={"tooltip-fb"} placement={"top"}>Facebook</Tooltip>}>
                                    <FacebookShareButton url={article.url} hashtag={"#CSCI_571_NewsApp"}>
                                        <FacebookIcon size={32} round={true}/>
                                    </FacebookShareButton>
                                </OverlayTrigger>

                                <OverlayTrigger overlay={<Tooltip id={"tooltip-tw"} placement={"top"}>Twitter</Tooltip>}>
                                    <TwitterShareButton url={article.url} hashtags={["CSCI_571_NewsApp"]}>
                                        <TwitterIcon size={32} round={true}/>
                                    </TwitterShareButton>
                                </OverlayTrigger>

                                <OverlayTrigger overlay={<Tooltip id={"tooltip-em"} placement={"top"}>Email</Tooltip>}>
                                    <EmailShareButton url={article.url} subject={"#CSCI_571_NewsApp"}>
                                        <EmailIcon size={32} round={true}/>
                                    </EmailShareButton>
                                </OverlayTrigger>

                                <OverlayTrigger overlay={<Tooltip id={"tooltip-bk"} placement={"top"}>Bookmark</Tooltip>}>
                                    <button onClick={() => this.handleBookMark(`${article.title}`)} style={{color: "red", fontSize: "31px"}}>
                                        {this.state.marked? <MdBookmark/>: <MdBookmarkBorder/>}
                                    </button>
                                </OverlayTrigger>
                            </span>
                        </h2>
                    </div>

                    <div className="img">
                        <img src={article.image} alt="" />
                    </div>

                    <div className={this.state.expanded? "fulltext": "truncated"} ref={this.myRef}>
                        {article.description}
                    </div>

                    <div className="expand">
                        <button onClick={ () => this.handleExpand(this.myRef) } style={{fontSize: "29px", fontStyle: "bold"}}>
                            {this.state.expanded? <MdKeyboardArrowUp/> : <MdKeyboardArrowDown/>}
                        </button>
                    </div>
                </div>

                <CommentBox id={this.state.id}/>
            </div>
          );
    }
}
 
export default Article;