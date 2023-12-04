<style scoped>

</style>

<template>
  <component
    :is="tag"
    :contenteditable="contenteditable"
    @input="update"
    @blur="update"
    @keypress="onKeypress"
    ref="element"
  >
  </component>
</template>

<script>


import { defineProps, ref, computed, onMounted, watch } from 'vue';

function replaceAll(st, searc, replacemen) {
  return str.split(search).join(replacement);
}
export default {
    props: {
        'tag': String,
        'contenteditable' : {
            type : [Boolean, String],
            default : true,
        },
        'modelValue': String,
        'noHtml' : {
            type : Boolean,
            default : true,
        },
        'noNl' : {
            type : Boolean,
            default : false,
        },
    },
    data() {
        return {
            
        }
    },
    watch: {
        modelValue(newval, oldval) {
            if(newval != this.currentContent()){
                this.updateContent(newval ?? '')
            }
        },
        noHtml(newval, oldval) {
            this.updateContent(props.modelValue ?? '')
        },
        tag: {
            handler(newVal, oldVal) {
                this.updateContent(this.modelValue || '');
            },
            deep: true,
        },
    },
    emits: ['returned', 'update:modelValue'],
    mounted() {
        this.updateContent(this.modelValue ?? '')
    },
    methods: {
        updateContent(newcontent){
            if(this.noHtml) {
                this.$refs.element.innerText = newcontent;
            }
            else {
                this.$refs.element.innerHTML = newcontent;
            }
        },
        currentContent(){
            return this.noHtml ? this.$refs.element.innerText : this.$refs.element.innerHTML;
        },
        update(event) {
            this.$emit('update:modelValue', this.currentContent());
        },
        // onPaste(event) {
        //     event.preventDefault();
        //     let text = (event.originalEvent || event).clipboardData.getData('text/plain');
        //     if(this.noNl) {
        //         text = replaceAll(text, '\r\n', ' ');
        //         text = replaceAll(text, '\n', ' ');
        //         text = replaceAll(text, '\r', ' ');
        //     }
        //     window.document.execCommand('insertText', false, text);
        // },
        onKeypress(event) {
            if(event.key == 'Enter' && this.noNl) {
                event.preventDefault();
                this.$emit('returned', this.currentContent());
            }
        }
    }
}


</script>