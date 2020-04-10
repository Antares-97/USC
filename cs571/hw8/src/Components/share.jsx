import React, { useState } from 'react';
import Modal from 'react-bootstrap/Modal';
import { MdShare } from "react-icons/md";
import {FacebookShareButton, 
        TwitterShareButton, 
        EmailShareButton,
        FacebookIcon,
        TwitterIcon,
        EmailIcon,
    } from 'react-share';


function ShareModal(props){
    const [show, setShow] = useState(false);

    function handleClose(){
        setShow(false);
    }
    
    function handleShow(e){
        e.stopPropagation();
        setShow(true);
    }

    return (
        <>
        <button onClick={handleShow}>
            <MdShare />
        </button>

        {show && 
        <div className="background" onClick={handleClose}>
            <div className="modal-share" onClick={event => event.stopPropagation()}>
                <Modal show={show} onHide={handleClose}>
                    <Modal.Header closeButton>
                        <Modal.Title>{props.title}</Modal.Title>
                    </Modal.Header>
                    <Modal.Body className="modal-body-central">
                        <p>Share Via</p>
                        <FacebookShareButton url={props.url} hashtag={"#CSCI_571_NewsApp"}>
                            <FacebookIcon size={48} round={true}/>
                        </FacebookShareButton>
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <TwitterShareButton url={props.url} hashtags={["CSCI_571_NewsApp"]}>
                            <TwitterIcon size={48} round={true}/>
                        </TwitterShareButton>
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <EmailShareButton url={props.url} subject={"#CSCI_571_NewsApp"}>
                            <EmailIcon size={48} round={true}/>
                        </EmailShareButton>
                    </Modal.Body>
                </Modal>
            </div>
        </div>
        }

        </>
    );

}

export default ShareModal;