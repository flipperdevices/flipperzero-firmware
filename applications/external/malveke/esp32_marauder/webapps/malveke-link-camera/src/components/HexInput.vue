<template>
    <contenteditable tag="div" :contenteditable="true" v-model="val" :no-nl="false" :no-html="true" class="textarea w-full h-full p-2.5 border bg-gray-50 border border-gray-300 text-gray-900 text-xs rounded-lg overflow-auto" />
</template>
<script>

import Contenteditable from './Contenteditable.vue';
import { ref, watch } from 'vue';
export default {
  name: "HexInput",
  props: ["modelValue"], //added the prop
  emits: ["update:modelValue"], //component emits the updated prop
  components: {
    Contenteditable
  },
  setup(props, { emit }) {
    const val = ref(props.modelValue);
    watch(val, (newValue) => {
      emit('update:modelValue', newValue);
    });
    return { emit , val};
  },
};
</script>
<style scoped>
.textarea {
  /* border: 1px solid #ccc; */
  font-family: inherit;
  font-size: inherit;
}
/* [contenteditable=true]:empty:before{
  @apply text-xs text-gray-400;
  content: attr(placeholder);
  pointer-events: none;
  display: block; 
} */
</style>
